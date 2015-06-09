#include "CheckerBoard.h"
#include <iostream>

CheckerBoard::CheckerBoard()
{
	Reset();
}

void CheckerBoard::Reset()
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

	m_playerTurn = true;
	m_forceJump = false;
	m_moveMade = false;
	m_multipleJumper = nullptr;

	m_gameState = UNKNOWN;

	m_player.Reset();
	m_ai.Reset();
}

CheckerBoard::~CheckerBoard()
{

}

void CheckerBoard::Create()
{
	m_camera.SetInputWindow(glfwGetCurrentContext());

	m_player.SetWindow(glfwGetCurrentContext());
	m_player.SetBoard(this);

	m_ai.SetBoard(this);
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
			else if (m_board[i][j] == SELECTED)
			{
				Gizmos::addCylinderFilled(glm::vec3((float)i, 0.2, (float)j), 0.4f, 0.1f, 8, glm::vec4(0.5, 0.5, 0, 1), &rotate);
			}
		}
		white = !white;
	}

	glm::vec2 cursor = *m_player.GetCursorPos();
	Gizmos::addSphere(vec3(cursor.x, 1.f, cursor.y), 0.2f, 10, 10, vec4(0, 0, 1, 1));

	Gizmos::draw(m_camera.GetProjectionView());
}

void CheckerBoard::Update(float dt)
{
	m_camera.Update(dt);

	CheckWinner();

	if (m_gameState == PLAYER)
	{
		std::cout << "PLAYER WINS!" << std::endl;
		Reset();
	}
	else if (m_gameState == AI)
	{
		std::cout << "AI WINS!" << std::endl;
		Reset();
	}

	if (m_playerTurn)
	{
		RemoveValidMoves();
		FindAllPotentialMoves(true);
		//wait for player input
		m_player.Update(dt);

		if (m_multipleJumper == NULL && m_moveMade)
		{
			SwitchTurn();
			m_moveMade = false;
		}

	}
	else
	{
		//ai make decision
		RemoveValidMoves();
		FindAllPotentialMoves(false);

		m_ai.Update(dt);

		if (m_multipleJumper == NULL && m_moveMade)
		{
			SwitchTurn();
			m_moveMade = false;
		}

	}
}

bool CheckerBoard::GetMultipleJumper()
{
	if (m_multipleJumper != NULL)
		return true;
	else return false;
}

bool CheckerBoard::GetPlayerTurn()
{
	return m_playerTurn;
}

bool CheckerBoard::GetMoveMade()
{
	return m_moveMade;
}

void CheckerBoard::SetMoveMade(bool moveMade)
{
	m_moveMade = moveMade;
}

void CheckerBoard::SetWinner(bool white)
{
	if (white)
		m_gameState = PLAYER;
	else
		m_gameState = AI;
}

GameState CheckerBoard::SimulateGame(glm::vec4 move, PieceType piece)
{
	int moveX = (int)move.x;
	int moveY = (int)move.y;
	int moveZ = (int)move.z;
	int moveW = (int)move.w;

	m_board[moveX][moveY] = EMPTY;
	m_board[moveZ][moveW] = piece;
	if (moveZ - moveX < -1 || moveZ - moveX  > 1)
	{
		if (moveX - moveZ == 2 && moveY - moveW == 2)
			m_board[moveX - 1][moveY - 1] = EMPTY;
		if (moveX - moveZ == 2 && moveY - moveW == -2)
			m_board[moveX - 1][moveY + 1] = EMPTY;
		if (moveX - moveZ == -2 && moveY - moveW == 2)
			m_board[moveX + 1][moveY - 1] = EMPTY;
		if (moveX - moveZ == -2 && moveY - moveW == -2)
			m_board[moveX + 1][moveY + 1] = EMPTY;
	}

	//update board state
	m_ai.SetBoardState(GetBoardState());
	m_player.SetBoardState(GetBoardState());

	CheckWinner();

	return GetWinState();
}

GameState CheckerBoard::GetWinState()
{
	return m_gameState;
}

void CheckerBoard::SwitchTurn()
{
	m_playerTurn = !m_playerTurn;
}

void CheckerBoard::CheckWinner()
{
	int whitecount = 0;
	int blackcount = 0;
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (m_board[i][j] == WHITE || m_board[i][j] == WHITEKING)
			{
				whitecount++;
			}
			else if (m_board[i][j] == BLACK || m_board[i][j] == BLACKKING)
			{
				blackcount++;
			}
		}
	}
	//if no white pieces ai wins
	if (whitecount == 0)
	{
		m_gameState = AI;
		return;
	}
	//if no black pieces player wins
	if (blackcount == 0)
	{
		m_gameState = PLAYER;
		return;
	}
	//else unkown
	m_gameState = UNKNOWN;
}

void CheckerBoard::SetMultipleJumper(Piece* multipleJumper)
{
	m_multipleJumper = multipleJumper;
}

void CheckerBoard::FindAllPotentialMoves(bool white)
{
	m_forceJump = false;

	if (m_multipleJumper != NULL) //check for a possible double jump
	{
		if (white) //if the players turn
		{
			//look through once to see if there any potential jumps to force the player into
			for (int i = 0; i < 8; ++i)
			{
				if (i == m_multipleJumper->m_x)
				{
					for (int j = 0; j < 8; ++j)
					{
						if (j == m_multipleJumper->m_y)
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
														if (m_board[x + 1][y - 1] == EMPTY || m_board[x + 1][y - 1] == VALID)
														{
															m_forceJump = true;
															continue;
														}
													}
													if (x - 1 >= 0 && x - 1 != i)
													{
														if (m_board[x - 1][y - 1] == EMPTY || m_board[x - 1][y - 1] == VALID)
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
														if (m_board[x + 1][y - 1] == EMPTY || m_board[x + 1][y - 1] == VALID)
														{
															m_forceJump = true;
															continue;
														}
													}
													if (x - 1 >= 0 && x - 1 != i)
													{
														if (m_board[x - 1][y - 1] == EMPTY || m_board[x - 1][y - 1] == VALID)
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
														if (m_board[x + 1][y + 1] == EMPTY || m_board[x + 1][y + 1] == VALID)
														{
															m_forceJump = true;
															continue;
														}
													}
													if (x - 1 >= 0 && x - 1 != i)
													{
														if (m_board[x - 1][y + 1] == EMPTY || m_board[x - 1][y + 1] == VALID)
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
				}
			}

			if (!m_forceJump)
			{
				//m_playerTurn = !m_playerTurn;
				m_multipleJumper = nullptr;
				return;
			}

			//look through again and mark valid moves
			for (int i = 0; i < 8; ++i)
			{
				if (i == m_multipleJumper->m_x)
				{
					for (int j = 0; j < 8; ++j)
					{
						if (j == m_multipleJumper->m_y)
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
													if (x - 1 >= 0 && x - 1 != i)
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
													if (x - 1 >= 0 && x - 1 != i)
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
													if (x - 1 >= 0 && x - 1 != i)
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
			}
		}
		else //if ai turn
		{
			//look through once to see if there any potential jumps to force the player into
			for (int i = 0; i < 8; ++i)
			{
				if (i == m_multipleJumper->m_x)
				{
					for (int j = 0; j < 8; ++j)
					{
						if (j == m_multipleJumper->m_y)
						{
							switch (m_board[i][j])
							{
							case BLACK:
							{
								for (int x = 0; x < 8; ++x)
								{
									for (int y = 0; y < 8; ++y)
									{
										if (x == i + 1 && y == j + 1 ||
											x == i - 1 && y == j + 1)
										{
											if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
											{
												//check if you can jump it
												if (y + 1 < 8)
												{
													if (x + 1 < 8 && x + 1 != i)
													{
														if (m_board[x + 1][y + 1] == EMPTY || m_board[x + 1][y + 1] == VALID)
														{
															m_forceJump = true;
															continue;
														}
													}
													if (x - 1 >= 0 && x - 1 != i)
													{
														if (m_board[x - 1][y + 1] == EMPTY || m_board[x - 1][y + 1] == VALID)
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
							case BLACKKING:
							{
								for (int x = 0; x < 8; ++x)
								{
									for (int y = 0; y < 8; ++y)
									{
										if (x == i + 1 && y == j + 1 ||
											x == i - 1 && y == j + 1)
										{
											if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
											{
												//check if you can jump it
												if (y + 1 < 8)
												{
													if (x + 1 < 8 && x + 1 != i)
													{
														if (m_board[x + 1][y + 1] == EMPTY || m_board[x + 1][y + 1] == VALID)
														{
															m_forceJump = true;
															continue;
														}
													}
													if (x - 1 >= 0 && x - 1 != i)
													{
														if (m_board[x - 1][y + 1] == EMPTY || m_board[x - 1][y + 1] == VALID)
														{
															m_forceJump = true;
															continue;
														}
													}
												}
											}
										}
										else if (x == i + 1 && y == j - 1 ||
											x == i - 1 && y == j - 1)
										{
											if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
											{
												//check if you can jump it
												if (y - 1 >= 0)
												{
													if (x + 1 < 8 && x + 1 != i)
													{
														if (m_board[x + 1][y - 1] == EMPTY || m_board[x + 1][y - 1] == VALID)
														{
															m_forceJump = true;
															continue;
														}
													}
													if (x - 1 >= 0 && x - 1 != i)
													{
														if (m_board[x - 1][y - 1] == EMPTY || m_board[x - 1][y - 1] == VALID)
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
				}
			}

			if (!m_forceJump)
			{
				//m_playerTurn = !m_playerTurn;
				m_multipleJumper = nullptr;
				return;
			}

			//look through again and mark valid moves
			for (int i = 0; i < 8; ++i)
			{
				if (i == m_multipleJumper->m_x)
				{
					for (int j = 0; j < 8; ++j)
					{
						if (j == m_multipleJumper->m_y)
						{
							switch (m_board[i][j])
							{
							case BLACK:
							{
								for (int x = 0; x < 8; ++x)
								{
									for (int y = 0; y < 8; ++y)
									{
										if (x == i + 1 && y == j + 1 ||
											x == i - 1 && y == j + 1)
										{
											if (m_board[x][y] == EMPTY && !m_forceJump)
											{
												m_board[x][y] = VALID;
											}
											if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
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
													if (x - 1 >= 0 && x - 1 != i)
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
							case BLACKKING:
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
											if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
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
													if (x - 1 >= 0 && x - 1 != i)
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
											if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
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
													if (x - 1 >= 0 && x - 1 != i)
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
			}
		}
	}
	else
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
												if (m_board[x + 1][y - 1] == EMPTY || m_board[x + 1][y - 1] == VALID)
												{
													m_forceJump = true;
													continue;
												}
											}
											if (x - 1 >= 0 && x - 1 != i)
											{
												if (m_board[x - 1][y - 1] == EMPTY || m_board[x - 1][y - 1] == VALID)
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
												if (m_board[x + 1][y - 1] == EMPTY || m_board[x + 1][y - 1] == VALID)
												{
													m_forceJump = true;
													continue;
												}
											}
											if (x - 1 >= 0 && x - 1 != i)
											{
												if (m_board[x - 1][y - 1] == EMPTY || m_board[x - 1][y - 1] == VALID)
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
												if (m_board[x + 1][y + 1] == EMPTY || m_board[x + 1][y + 1] == VALID)
												{
													m_forceJump = true;
													continue;
												}
											}
											if (x - 1 >= 0 && x - 1 != i)
											{
												if (m_board[x - 1][y + 1] == EMPTY || m_board[x - 1][y + 1] == VALID)
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
											if (x - 1 >= 0 && x - 1 != i)
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
											if (x - 1 >= 0 && x - 1 != i)
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
											if (x - 1 >= 0 && x - 1 != i)
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
			//look through once to see if there any potential jumps to force the player into
			for (int i = 0; i < 8; ++i)
			{
				for (int j = 0; j < 8; ++j)
				{
					switch (m_board[i][j])
					{
					case BLACK:
					{
						for (int x = 0; x < 8; ++x)
						{
							for (int y = 0; y < 8; ++y)
							{
								if (x == i + 1 && y == j + 1 ||
									x == i - 1 && y == j + 1)
								{
									if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
									{
										//check if you can jump it
										if (y + 1 < 8)
										{
											if (x + 1 < 8 && x + 1 != i)
											{
												if (m_board[x + 1][y + 1] == EMPTY || m_board[x + 1][y + 1] == VALID)
												{
													m_forceJump = true;
													continue;
												}
											}
											if (x - 1 >= 0 && x - 1 != i)
											{
												if (m_board[x - 1][y + 1] == EMPTY || m_board[x - 1][y + 1] == VALID)
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
					case BLACKKING:
					{
						for (int x = 0; x < 8; ++x)
						{
							for (int y = 0; y < 8; ++y)
							{
								if (x == i + 1 && y == j + 1 ||
									x == i - 1 && y == j + 1)
								{
									if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
									{
										//check if you can jump it
										if (y + 1 < 8)
										{
											if (x + 1 < 8 && x + 1 != i)
											{
												if (m_board[x + 1][y + 1] == EMPTY || m_board[x + 1][y + 1] == VALID)
												{
													m_forceJump = true;
													continue;
												}
											}
											if (x - 1 >= 0 && x - 1 != i)
											{
												if (m_board[x - 1][y + 1] == EMPTY || m_board[x - 1][y + 1] == VALID)
												{
													m_forceJump = true;
													continue;
												}
											}
										}
									}
								}
								else if (x == i + 1 && y == j - 1 ||
									x == i - 1 && y == j - 1)
								{
									if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
									{
										//check if you can jump it
										if (y - 1 >= 0)
										{
											if (x + 1 < 8 && x + 1 != i)
											{
												if (m_board[x + 1][y - 1] == EMPTY || m_board[x + 1][y - 1] == VALID)
												{
													m_forceJump = true;
													continue;
												}
											}
											if (x - 1 >= 0 && x - 1 != i)
											{
												if (m_board[x - 1][y - 1] == EMPTY || m_board[x - 1][y - 1] == VALID)
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
					case BLACK:
					{
						for (int x = 0; x < 8; ++x)
						{
							for (int y = 0; y < 8; ++y)
							{
								if (x == i + 1 && y == j + 1 ||
									x == i - 1 && y == j + 1)
								{
									if (m_board[x][y] == EMPTY && !m_forceJump)
									{
										m_board[x][y] = VALID;
									}
									if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
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
											if (x - 1 >= 0 && x - 1 != i)
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
					case BLACKKING:
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
									if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
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
											if (x - 1 >= 0 && x - 1 != i)
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
									if (m_board[x][y] == WHITE || m_board[x][y] == WHITEKING)
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
											if (x - 1 >= 0 && x - 1 != i)
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

Piece* CheckerBoard::GetPlayerPieces()
{
	return m_player.GetPieces();
}

Piece* CheckerBoard::GetAIPieces()
{
	return m_ai.GetPieces();
}

void CheckerBoard::Destroy()
{

}

void CheckerBoard::ClearBoard()
{

}