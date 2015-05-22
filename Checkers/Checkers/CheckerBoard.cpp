#include "CheckerBoard.h"

CheckerBoard::CheckerBoard()
{
	bool white = true;
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (!white && j < 3)
				m_board[i][j] = BLACK;
			else if (!white && j < 5)
				m_board[i][j] = EMPTY;
			else if (!white && j < 8)
				m_board[i][j] = WHITE;

			white = !white;
		}
		white = !white;
	}

	m_board[3][4] = BLACK;

	m_playerTurn = true;
	m_forceJump = false;
}

CheckerBoard::~CheckerBoard()
{

}

void CheckerBoard::Create()
{
	m_camera.SetInputWindow(glfwGetCurrentContext());

	m_ai = new CheckerAI();
	m_player = new CheckerPlayer();

	m_player->SetWindow(glfwGetCurrentContext());
	m_player->SetBoard(this);
}

void CheckerBoard::Draw()
{
	glm::mat4 rotate = glm::rotate(mat4(1.0f), glm::pi<float>() / 4, glm::vec3(0, 1, 0));
	
	Gizmos::addCylinderFilled(glm::vec3(3.5f, -0.1, 3.5f), 6.f, 0.2f, 4, glm::vec4(139.f / 255.f, 69.f / 255.f, 19.f / 255.f, 1), &rotate);

	bool white = true;
	
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (white)
			{
				Gizmos::addCylinderFilled(glm::vec3((float)i, 0, (float)j), 0.6f, 0.2f, 4, glm::vec4(1, 1, 1, 1), &rotate);
			}
			else
			{
				Gizmos::addCylinderFilled(glm::vec3((float)i, 0, (float)j), 0.6f, 0.2f, 4, glm::vec4(0, 0, 0, 1), &rotate);
			}
			white = !white;

			if (m_board[i][j] == WHITE)
			{
				Gizmos::addCylinderFilled(glm::vec3((float)i, 0.2, (float)j), 0.4f, 0.1f, 8, glm::vec4(0.8, 0.8, 0.8, 1), &rotate);
			}
			else if (m_board[i][j] == WHITEKING)
			{
				Gizmos::addCylinderFilled(glm::vec3((float)i, 0.2, (float)j), 0.4f, 0.2f, 8, glm::vec4(0.8, 0.8, 0.8, 1), &rotate);
			}
			else if (m_board[i][j] == BLACK)
			{
				Gizmos::addCylinderFilled(glm::vec3((float)i, 0.2, (float)j), 0.4f, 0.1f, 8, glm::vec4(0.2, 0.2, 0.2, 1), &rotate);
			}
			else if (m_board[i][j] == BLACKKING)
			{
				Gizmos::addCylinderFilled(glm::vec3((float)i, 0.2, (float)j), 0.4f, 0.2f, 8, glm::vec4(0.2, 0.2, 0.2, 1), &rotate);
			}
			else if (m_board[i][j] == VALID)
			{
				Gizmos::addCylinderFilled(glm::vec3((float)i, 0.2, (float)j), 0.4f, 0.1f, 8, glm::vec4(0.2, 0.8, 0.2, 1), &rotate);
			}
		}
		white = !white;
	}

	glm::vec2 cursor = *m_player->GetCursorPos();
	Gizmos::addSphere(vec3(cursor.x, 1.f, cursor.y), 0.2f, 10, 10, vec4(0, 0, 1, 1));

	Gizmos::draw(m_camera.GetProjectionView());
}


void CheckerBoard::Update(float dt)
{
	m_camera.Update(dt);

	//for (int i = 0; i < 8; ++i)
	//{
	//	for (int j = 0; j < 8; ++j)
	//	{
	//		if (m_board[i][j] == VALID)
	//		{
	//			m_board[i][j] = EMPTY;
	//		}
	//	}
	//}

	if (m_playerTurn)
	{
		FindAllPotentialMoves(true);
		//wait for player input
		glm::vec4 pieceChange = *m_player->Update(dt);
		if (!(pieceChange.x == 0 && pieceChange.y == 0))
		{
			m_board[(int)pieceChange.x][(int)pieceChange.y] = EMPTY;
			m_board[(int)pieceChange.z][(int)pieceChange.w] = WHITE;
			m_playerTurn = false;
		}
	}
	else
	{
		//ai make decision
		m_playerTurn = !m_playerTurn;
	}
}

void CheckerBoard::CheckValidMoves(glm::vec2 piece)
{
	if (m_board[(int)piece.x][(int)piece.y] != EMPTY)
	{
		switch (m_board[(int)piece.x][(int)piece.y])
		{
		case WHITE:
		{
					  for (int i = 0; i < 8; ++i)
					  {
						  for (int j = 0; j < 8; ++j)
						  {
							  if (i == piece.x + 1 && j == piece.y - 1 ||
								  i == piece.x - 1 && j == piece.y - 1)
							  {
								  if (m_board[i][j] == EMPTY)
								  {
									  m_board[i][j] = VALID;
								  }
								  if (m_board[i][j] == BLACK || m_board[i][j] == BLACKKING)
								  {
									  //check if you can jump it
									  if (j - 1 >= 0)
									  {
										  if (i + 1 < 8 && i + 1 != piece.x)
										  {
											  if (m_board[i + 1][j - 1] == EMPTY)
												  m_board[i + 1][j - 1] = VALID;
										  }
										  if (i - 1 > 0 && i - 1 != piece.x)
										  {
											  if (m_board[i - 1][j - 1] == EMPTY)
												  m_board[i - 1][j - 1] = VALID;
										  }

									  }
									  //set forceJump to true
									  m_forceJump = true;
								  }
							  }
						  }
					  }
			break;
		}
		case BLACK:
		{
			break;
		}
		case WHITEKING:
		{
			break;
		}
		case BLACKKING:
		{
			break;
		}
		default:
			break;
		}
	}
}

void CheckerBoard::FindAllPotentialMoves(bool white)
{
	if (white) //if the players turn
	{
		//look through once to see if there any potential jumps to force the player into
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				switch (m_board[i][j])
				{
				case WHITE:
				{
					for (int x = 0; x < 8; ++x)
					{
					  for (int y = 0; y < 8; ++y)
					  {
						  if (x == i + 1 && y == j - 1 ||
							  x == i - 1 && y == j - 1)
						  {
							  if (m_board[x][y] == BLACK || m_board[x][y] == BLACKKING)
							  {
								  //check if you can jump it
								  if (y - 1 >= 0)
								  {
									  if (x + 1 < 8 && x + 1 != i)
									  {
										  if (m_board[x + 1][y - 1] == EMPTY)
										  {
											  m_forceJump = true;
											  continue;
										  }
									  }
									  if (x - 1 > 0 && x - 1 != i)
									  {
										  if (m_board[x - 1][y - 1] == EMPTY)
										  {
											  m_forceJump = true;
											  continue;
										  }
									  }
								  }
							  }
						  }
					  }
					}
					break;
				}
				case WHITEKING:
				{
					for (int x = 0; x < 8; ++x)
					{
					  for (int y = 0; y < 8; ++y)
					  {
						  if (x == i + 1 && y == j - 1 ||
							  x == i - 1 && y == j - 1)
						  {
							  if (m_board[x][y] == BLACK || m_board[x][y] == BLACKKING)
							  {
								  //check if you can jump it
								  if (y - 1 >= 0)
								  {
									  if (x + 1 < 8 && x + 1 != i)
									  {
										  if (m_board[x + 1][y - 1] == EMPTY)
										  {
											  m_forceJump = true;
											  continue;
										  }
									  }
									  if (x - 1 > 0 && x - 1 != i)
									  {
										  if (m_board[x - 1][y - 1] == EMPTY)
										  {
											  m_forceJump = true;
											  continue;
										  }
									  }
								  }
							  }
						  }
						  else if (x == i + 1 && y == j + 1 ||
							  x == i - 1 && y == j + 1)
						  {
							  if (m_board[x][y] == BLACK || m_board[x][y] == BLACKKING)
							  {
								  //check if you can jump it
								  if (y + 1 < 8)
								  {
									  if (x + 1 < 8 && x + 1 != i)
									  {
										  if (m_board[x + 1][y + 1] == EMPTY)
										  {
											  m_forceJump = true;
											  continue;
										  }
									  }
									  if (x - 1 > 0 && x - 1 != i)
									  {
										  if (m_board[x - 1][y + 1] == EMPTY)
										  {
											  m_forceJump = true;
											  continue;
										  }
									  }
								  }
							  }
						  }
					  }
					}
					break;
				}
				default:
					break;
				}
			}
		}

		//look through again and mark valid moves
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				switch (m_board[i][j])
				{
				case WHITE:
				{
					for (int x = 0; x < 8; ++x)
					{
					  for (int y = 0; y < 8; ++y)
					  {
						  if (x == i + 1 && y == j - 1 ||
							  x == i - 1 && y == j - 1)
						  {
							  if (m_board[x][y] == EMPTY && !m_forceJump)
							  {
								  m_board[x][y] = VALID;
							  }
							  if (m_board[x][y] == BLACK || m_board[x][y] == BLACKKING)
							  {
								  //check if you can jump it
								  if (y - 1 >= 0)
								  {
									  if (x + 1 < 8 && x + 1 != i)
									  {
										  if (m_board[x + 1][y - 1] == EMPTY)
											  m_board[x + 1][y - 1] = VALID;
										  //---------------------------------------------------check for consecutive jumps
									  }
									  if (x - 1 > 0 && x - 1 != i)
									  {
										  if (m_board[x - 1][y - 1] == EMPTY)
											  m_board[x - 1][y - 1] = VALID;
										  //---------------------------------------------------check for consecutive jumps
									  }
								  }
							  }
						  }
					  }
					}
					break;
				}
				case WHITEKING:
				{
					for (int x = 0; x < 8; ++x)
					{
					  for (int y = 0; y < 8; ++y)
					  {
						  if (x == i + 1 && y == j - 1 ||
							  x == i - 1 && y == j - 1)
						  {
							  if (m_board[x][y] == EMPTY && !m_forceJump)
							  {
								  m_board[x][y] = VALID;
							  }
							  if (m_board[x][y] == BLACK || m_board[x][y] == BLACKKING)
							  {
								  //check if you can jump it
								  if (y - 1 >= 0)
								  {
									  if (x + 1 < 8 && x + 1 != i)
									  {
										  if (m_board[x + 1][y - 1] == EMPTY)
											  m_board[x + 1][y - 1] = VALID;
										  //---------------------------------------------------check for consecutive jumps
									  }
									  if (x - 1 > 0 && x - 1 != i)
									  {
										  if (m_board[x - 1][y - 1] == EMPTY)
											  m_board[x - 1][y - 1] = VALID;
										  //---------------------------------------------------check for consecutive jumps
									  }

								  }
							  }
						  }
						  else if (x == i + 1 && y == j + 1 ||
							  x == i - 1 && y == j + 1)
						  {
							  if (m_board[x][y] == EMPTY && !m_forceJump)
							  {
								  m_board[x][y] = VALID;
							  }
							  if (m_board[x][y] == BLACK || m_board[x][y] == BLACKKING)
							  {
								  //check if you can jump it
								  if (y + 1 < 8)
								  {
									  if (x + 1 < 8 && x + 1 != i)
									  {
										  if (m_board[x + 1][y + 1] == EMPTY)
											  m_board[x + 1][y + 1] = VALID;
										  //---------------------------------------------------check for consecutive jumps
									  }
									  if (x - 1 > 0 && x - 1 != i)
									  {
										  if (m_board[x - 1][y + 1] == EMPTY)
											  m_board[x - 1][y + 1] = VALID;
										  //---------------------------------------------------check for consecutive jumps
									  }

								  }
							  }
						  }
					  }
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}
	else //if ai turn
	{

	}
}

PieceType** CheckerBoard::GetBoardState()
{
	PieceType** thing = new PieceType*[8];
	for (int i = 0; i < 8; ++i)
	{
		thing[i] = new PieceType[8];
		for (int j = 0; j < 8; j++)
		{
			thing[i][j] = m_board[i][j];
		}
	}

	return thing;
}

bool CheckerBoard::GetForceJump()
{
	return m_forceJump;
}

void CheckerBoard::RemoveValidMoves()
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (m_board[i][j] == VALID)
			{
				m_board[i][j] = EMPTY;
			}
		}
	}
}

void CheckerBoard::Destroy()
{

}

void CheckerBoard::ClearBoard()
{

}