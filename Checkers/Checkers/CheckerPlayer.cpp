#include "CheckerPlayer.h"
#include "CheckerBoard.h"


CheckerPlayer::CheckerPlayer()
{
	m_cursor.x = 0;
	m_cursor.y = 0;

	bool second = true;
	int k = 0;
	for (int i = 5; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (second)
			{
				m_pieces[k].m_x = j;
				m_pieces[k].m_y = i;
				m_pieces[k].m_alive = true;
				m_pieces[k].m_type = false;
				k++;
			}
			second = !second;
		}
		second = !second;
	}

	m_selectCooldown = 0;
}

CheckerPlayer::~CheckerPlayer()
{

}

//player needs to know about enemy pieces to check for jumps
void CheckerPlayer::Update(float dt)
{
	if (m_selectCooldown < 0.0f)
	{
		m_selectCooldown = 0.1f;
		//move cursor
		if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			if (m_cursor.y > 0)
				m_cursor.y -= 1;
		}

		if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			if (m_cursor.y < 7)
				m_cursor.y += 1;
		}

		if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			if (m_cursor.x > 0)
				m_cursor.x -= 1;
		}

		if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			if (m_cursor.x < 7)
				m_cursor.x += 1;
		}

		if (glfwGetKey(m_window, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			//m_board->FindAllPotentialMoves(true);
			PieceType** tempBoard = m_board->GetBoardState();

			if (m_currentPiece != NULL)
			{
				for (int i = 0; i < 12; ++i)
				{
					if (m_pieces[i].m_x == m_cursor.x && m_pieces[i].m_y == m_cursor.y && m_pieces[i].m_alive == true)
					{
						m_currentPiece = &m_pieces[i];
						return;
					}
				}

				if (!m_currentPiece->m_type) // not a king
				{
					for (int i = 0; i < 8; ++i)
					{
						for (int j = 0; j < 8; ++j)
						{
							if (i == (int)m_cursor.x && j == (int)m_cursor.y)
							{
								if (tempBoard[i][j] == VALID)
								{
									if (i == m_currentPiece->m_x + 1 && j == m_currentPiece->m_y - 1 ||
										i == m_currentPiece->m_x - 1 && j == m_currentPiece->m_y - 1)
									{
										//move piece and end turn
										if (j == 0)
										{
											m_currentPiece->m_type = true;
										}

										PieceType currentPieceType;
										if (m_currentPiece->m_type)
											currentPieceType = WHITEKING;
										else
											currentPieceType = WHITE;

										glm::vec4 move = glm::vec4(m_currentPiece->m_x, m_currentPiece->m_y, i, j);
										m_currentPiece->m_x = i;
										m_currentPiece->m_y = j;
										m_board->SimulateGame(move, currentPieceType);

										m_currentPiece = nullptr;

										m_board->SetMoveMade(true);
										return;
									}
									if (i == m_currentPiece->m_x + 2 && j == m_currentPiece->m_y - 2 ||
										i == m_currentPiece->m_x - 2 && j == m_currentPiece->m_y - 2)
									{
										//move piece and end turn
										if (j == 0)
										{
											m_currentPiece->m_type = true;
										}

										PieceType currentPieceType;
										if (m_currentPiece->m_type)
											currentPieceType = WHITEKING;
										else
											currentPieceType = WHITE;
										
										glm::vec4 move = glm::vec4(m_currentPiece->m_x, m_currentPiece->m_y, i, j);
										m_currentPiece->m_x = i;
										m_currentPiece->m_y = j;
										m_board->SimulateGame(move, currentPieceType);

										m_board->SetMultipleJumper(m_currentPiece);

										m_currentPiece = nullptr;

										m_board->SetMoveMade(true);
										return;
									}
								}
							}
						}
					}
				}
				else //a king
				{
					for (int i = 0; i < 8; ++i)
					{
						for (int j = 0; j < 8; ++j)
						{
							if (i == (int)m_cursor.x && j == (int)m_cursor.y)
							{
								if (tempBoard[i][j] == VALID)
								{
									if (i == m_currentPiece->m_x + 1 && j == m_currentPiece->m_y - 1 ||
										i == m_currentPiece->m_x - 1 && j == m_currentPiece->m_y - 1)
									{
										//move piece and end turn
										if (j == 0)
										{
											m_currentPiece->m_type = true;
										}

										PieceType currentPieceType;
										if (m_currentPiece->m_type)
											currentPieceType = WHITEKING;
										else
											currentPieceType = WHITE;

										glm::vec4 move = glm::vec4(m_currentPiece->m_x, m_currentPiece->m_y, i, j);
										m_currentPiece->m_x = i;
										m_currentPiece->m_y = j;
										m_board->SimulateGame(move, currentPieceType);

										m_currentPiece = nullptr;

										m_board->SetMoveMade(true);
										return;
									}
									if (i == m_currentPiece->m_x + 1 && j == m_currentPiece->m_y + 1 ||
										i == m_currentPiece->m_x - 1 && j == m_currentPiece->m_y + 1)
									{
										//move piece and end turn
										if (j == 0)
										{
											m_currentPiece->m_type = true;
										}

										PieceType currentPieceType;
										if (m_currentPiece->m_type)
											currentPieceType = WHITEKING;
										else
											currentPieceType = WHITE;

										glm::vec4 move = glm::vec4(m_currentPiece->m_x, m_currentPiece->m_y, i, j);
										m_currentPiece->m_x = i;
										m_currentPiece->m_y = j;
										m_board->SimulateGame(move, currentPieceType);

										m_currentPiece = nullptr;

										m_board->SetMoveMade(true);
										return;
									}

									if (i == m_currentPiece->m_x + 2 && j == m_currentPiece->m_y - 2 ||
										i == m_currentPiece->m_x - 2 && j == m_currentPiece->m_y - 2)
									{
										//move piece and end turn
										if (j == 0)
										{
											m_currentPiece->m_type = true;
										}

										PieceType currentPieceType;
										if (m_currentPiece->m_type)
											currentPieceType = WHITEKING;
										else
											currentPieceType = WHITE;

										glm::vec4 move = glm::vec4(m_currentPiece->m_x, m_currentPiece->m_y, i, j);
										m_currentPiece->m_x = i;
										m_currentPiece->m_y = j;
										m_board->SimulateGame(move, currentPieceType);

										m_board->SetMultipleJumper(m_currentPiece);

										m_currentPiece = nullptr;

										m_board->SetMoveMade(true);
										return;
									}
									if (i == m_currentPiece->m_x + 2 && j == m_currentPiece->m_y + 2 ||
										i == m_currentPiece->m_x - 2 && j == m_currentPiece->m_y + 2)
									{
										//move piece and end turn
										if (j == 0)
										{
											m_currentPiece->m_type = true;
										}

										PieceType currentPieceType;
										if (m_currentPiece->m_type)
											currentPieceType = WHITEKING;
										else
											currentPieceType = WHITE;

										glm::vec4 move = glm::vec4(m_currentPiece->m_x, m_currentPiece->m_y, i, j);
										m_currentPiece->m_x = i;
										m_currentPiece->m_y = j;
										m_board->SimulateGame(move, currentPieceType);

										m_board->SetMultipleJumper(m_currentPiece);

										m_currentPiece = nullptr;

										m_board->SetMoveMade(true);
										return;
									}
								}
							}
						}
					}
				}
				
			}
			else
			{
				for (int i = 0; i < 12; ++i)
				{
					if (m_pieces[i].m_x == m_cursor.x && m_pieces[i].m_y == m_cursor.y && m_pieces[i].m_alive)
					{
						m_currentPiece = &m_pieces[i];
						break;
					}
				}
			}
		}
	}

	//if (m_currentPiece != NULL)
	//{
	//	m_board->CheckValidMoves(glm::vec2(m_currentPiece->m_x, m_currentPiece->m_y));
	//}

	m_selectCooldown -= dt;
	return;
}

void CheckerPlayer::SetBoardState(PieceType** currentState)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			for (int x = 0; x < 12; ++x)
			{
				if (m_pieces[x].m_x == i && m_pieces[x].m_y == j)
				{
					if (currentState[i][j] == EMPTY)
						m_pieces[x].m_alive = false;
				}
			}
		}
	}
}

Piece* CheckerPlayer::GetPiece(int i)
{
	return &m_pieces[i];
}

Piece* CheckerPlayer::GetPieceAtPos(int x, int y)
{
	for (int k = 0; k < 12; ++k)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 8; j++)
			{
				if (m_pieces[k].m_x == i && m_pieces[k].m_y == j)
				{
					return &m_pieces[k];
				}
			}
		}
	}

	return nullptr;
}

glm::vec2* CheckerPlayer::GetCursorPos()
{
	return &m_cursor;
}

void CheckerPlayer::SetWindow(GLFWwindow* window)
{
	m_window = window;
}

void CheckerPlayer::SetBoard(CheckerBoard* board)
{
	m_board = board;
}

Piece* CheckerPlayer::GetPieces()
{
	return &m_pieces[0];
}