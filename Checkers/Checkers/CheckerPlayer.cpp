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
glm::vec4* CheckerPlayer::Update(float dt)
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
					if (m_pieces[i].m_x == m_cursor.x && m_pieces[i].m_y == m_cursor.y)
					{
						m_currentPiece = &m_pieces[i];
						return &glm::vec4(0);
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
								if (i == m_currentPiece->m_x + 1 && j == m_currentPiece->m_y - 1 ||
									i == m_currentPiece->m_x - 1 && j == m_currentPiece->m_y - 1)
								{
									if (tempBoard[i][j] == VALID)
									{
										//move piece and end turn
										glm::vec2 oldPos(m_currentPiece->m_x, m_currentPiece->m_y);

										m_currentPiece->m_x = j;
										m_currentPiece->m_y = i;
										if (m_currentPiece->m_y == 0)
										{
											m_currentPiece->m_type == true;
										}
										m_currentPiece = nullptr;
										m_board->RemoveValidMoves();
										return &glm::vec4(oldPos.x, oldPos.y, j, i);

									}
									if (tempBoard[i][j] == BLACK || tempBoard[i][j] == BLACKKING)
									{
										//check if you can jump it
										if (j - 1 >= 0)
										{
											if (i + 1 < 8 && i + 1 != m_currentPiece->m_x)
											{
												if (tempBoard[i + 1][j - 1] == VALID)
												{
													//move piece and end turn
													glm::vec2 oldPos(m_currentPiece->m_x, m_currentPiece->m_y);

													m_currentPiece->m_x = j - 1;
													m_currentPiece->m_y = i + 1;
													if (m_currentPiece->m_y == 0)
													{
														m_currentPiece->m_type == true;
													}
													m_currentPiece = nullptr;
													m_board->RemoveValidMoves();
													return &glm::vec4(oldPos.x, oldPos.y, j - 1, i + 1);
													//check for consecutive jumps
												}
											}
											if (i - 1 >= 0 && i - 1 != m_currentPiece->m_x)
											{
												if (tempBoard[i - 1][j - 1] == VALID)
												{
													//move piece and end turn
													glm::vec2 oldPos(m_currentPiece->m_x, m_currentPiece->m_y);

													m_currentPiece->m_x = j - 1;
													m_currentPiece->m_y = i - 1;
													if (m_currentPiece->m_y == 0)
													{
														m_currentPiece->m_type == true;
													}
													m_currentPiece = nullptr;
													m_board->RemoveValidMoves();
													return &glm::vec4(oldPos.x, oldPos.y, j - 1, i - 1);
													//check for consecutive jumps
												}
											}
										}
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
								if (i == m_currentPiece->m_x + 1 && j == m_currentPiece->m_y - 1 ||
									i == m_currentPiece->m_x - 1 && j == m_currentPiece->m_y - 1)
								{
									if (tempBoard[i][j] == VALID)
									{
										//move piece and end turn
										glm::vec2 oldPos(m_currentPiece->m_x, m_currentPiece->m_y);

										m_currentPiece->m_x = j;
										m_currentPiece->m_y = i;
										if (m_currentPiece->m_y == 0)
										{
											m_currentPiece->m_type == true;
										}
										m_currentPiece = nullptr;
										m_board->RemoveValidMoves();
										return &glm::vec4(oldPos.x, oldPos.y, j, i);

									}
									if (tempBoard[i][j] == BLACK || tempBoard[i][j] == BLACKKING)
									{
										//check if you can jump it
										if (j - 1 >= 0)
										{
											if (i + 1 < 8 && i + 1 != m_currentPiece->m_x)
											{
												if (tempBoard[i + 1][j - 1] == VALID)
												{
													//move piece and end turn
													glm::vec2 oldPos(m_currentPiece->m_x, m_currentPiece->m_y);

													m_currentPiece->m_x = j - 1;
													m_currentPiece->m_y = i + 1;
													if (m_currentPiece->m_y == 0)
													{
														m_currentPiece->m_type == true;
													}
													m_currentPiece = nullptr;
													m_board->RemoveValidMoves();
													return &glm::vec4(oldPos.x, oldPos.y, j - 1, i + 1);
													//check for consecutive jumps
												}
											}
											if (i - 1 > 0 && i - 1 != m_currentPiece->m_x)
											{
												if (tempBoard[i - 1][j - 1] == VALID)
												{
													//move piece and end turn
													glm::vec2 oldPos(m_currentPiece->m_x, m_currentPiece->m_y);

													m_currentPiece->m_x = j - 1;
													m_currentPiece->m_y = i - 1;
													if (m_currentPiece->m_y == 0)
													{
														m_currentPiece->m_type == true;
													}
													m_currentPiece = nullptr;
													m_board->RemoveValidMoves();
													return &glm::vec4(oldPos.x, oldPos.y, j - 1, i - 1);
													//check for consecutive jumps
												}
											}
										}
									}
								}
								else if (i == m_currentPiece->m_x + 1 && j == m_currentPiece->m_y + 1 ||
									i == m_currentPiece->m_x - 1 && j == m_currentPiece->m_y + 1)
								{
									if (tempBoard[i][j] == VALID)
									{
										//move piece and end turn
										glm::vec2 oldPos(m_currentPiece->m_x, m_currentPiece->m_y);

										m_currentPiece->m_x = j;
										m_currentPiece->m_y = i;
										if (m_currentPiece->m_y == 0)
										{
											m_currentPiece->m_type == true;
										}
										m_currentPiece = nullptr;
										m_board->RemoveValidMoves();
										return &glm::vec4(oldPos.x, oldPos.y, j, i);

									}
									if (tempBoard[i][j] == BLACK || tempBoard[i][j] == BLACKKING)
									{
										//check if you can jump it
										if (j + 1 < 8)
										{
											if (i + 1 < 8 && i + 1 != m_currentPiece->m_x)
											{
												if (tempBoard[i + 1][j + 1] == VALID)
												{
													//move piece and end turn
													glm::vec2 oldPos(m_currentPiece->m_x, m_currentPiece->m_y);

													m_currentPiece->m_x = j + 1;
													m_currentPiece->m_y = i + 1;
													if (m_currentPiece->m_y == 0)
													{
														m_currentPiece->m_type == true;
													}
													m_currentPiece = nullptr;
													m_board->RemoveValidMoves();
													return &glm::vec4(oldPos.x, oldPos.y, j + 1, i + 1);
													//check for consecutive jumps
												}
											}
											if (i - 1 > 0 && i - 1 != m_currentPiece->m_x)
											{
												if (tempBoard[i - 1][j + 1] == VALID)
												{
													//move piece and end turn
													glm::vec2 oldPos(m_currentPiece->m_x, m_currentPiece->m_y);

													m_currentPiece->m_x = j + 1;
													m_currentPiece->m_y = i - 1;
													if (m_currentPiece->m_y == 0)
													{
														m_currentPiece->m_type == true;
													}
													m_currentPiece = nullptr;
													m_board->RemoveValidMoves();
													return &glm::vec4(oldPos.x, oldPos.y, j + 1, i - 1);
													//check for consecutive jumps
												}
											}
										}
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
					if (m_pieces[i].m_x == m_cursor.x && m_pieces[i].m_y == m_cursor.y)
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
	return &glm::vec4(0);
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

