#include "CheckerAI.h"
#include "CheckerBoard.h"

#include <vector>
#include <time.h>
#include <cstdlib>

CheckerAI::CheckerAI()
{
	srand(time(NULL));
}

CheckerAI::~CheckerAI()
{

}

glm::vec4* CheckerAI::Update(float dt)
{
	PieceType** board = m_board->GetBoardState();

	std::vector<float> actionScores;

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (board[i][j] == VALID)
			{
				int value = 0;

				Piece* currentPiece = &m_pieces[0];

				//find piece associated with move
				for (int x = 0; x < 12; ++x)
				{
					if (m_pieces[x].m_type == false)
					{
						if (m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j - 1 ||
							m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j - 1)
						{
							currentPiece = &m_pieces[x];
						}

						if ((board[i - 1][j - 1] == WHITE || board[i - 1][j - 1] == WHITEKING)) //////////////////////
						{
							if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j - 2)
							{
								currentPiece = &m_pieces[x];
							}
						}
						if ((board[i + 1][j - 1] == WHITE || board[i + 1][j - 1] == WHITEKING))
						{
							if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j - 2)
							{
								currentPiece = &m_pieces[x];
							}
						}
						else //a king
						{

							if (m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j + 1 ||
								m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j + 1 ||
								m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j - 1 ||
								m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j - 1)
							{
								currentPiece = &m_pieces[x];
							}

							if ((board[i - 1][j - 1] == WHITE || board[i - 1][j - 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j - 2)
								{
									currentPiece = &m_pieces[x];
								}
							}
							if ((board[i + 1][j - 1] == WHITE || board[i + 1][j - 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j - 2)
								{
									currentPiece = &m_pieces[x];
								}
							}
							if ((board[i - 1][j + 1] == WHITE || board[i - 1][j + 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j + 2)
								{
									currentPiece = &m_pieces[x];
								}
							}
							if ((board[i + 1][j + 1] == WHITE || board[i + 1][j + 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j + 2)
								{
									currentPiece = &m_pieces[x];
								}
							}
						}
					}
				}
				//piece associated with move found
				//stored in currentPiece

				for (int playout = 0; playout < 20; ++playout)
				{
					//clone the game
					CheckerBoard clone = *m_board;

					PieceType currentPieceType;
					if (currentPiece->m_type)
						currentPieceType = BLACKKING;
					else
						currentPieceType = BLACK;

					//perform action
					clone.SimulateGame(glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j), currentPieceType);

					bool playerTurn = true;
					//while no winner simulate rest of game
					while (clone.GetWinState() != UNKNOWN)
					{
						clone.FindAllPotentialMoves(playerTurn);

						bool boardValid = false;
						int n;
						int m;
						while (!boardValid) //choose random valid move
						{
							n = rand() % 8;
							m = rand() % 8;

							if (board[n][m] == VALID)
								boardValid = true;
						}

						PieceType basic;
						PieceType king;

						if (playerTurn)
						{
							basic = BLACK;
							king = BLACKKING;
						}
						else
						{
							basic = WHITE;
							king = WHITEKING;
						}

						Piece* nextCurrentPiece = &m_pieces[0];

						//find piece associated with move
						for (int x = 0; x < 12; ++x)
						{
							if (m_pieces[x].m_type == false)
							{
								if (m_pieces[x].m_x == n - 1 && m_pieces[x].m_y == m - 1 ||
									m_pieces[x].m_x == n + 1 && m_pieces[x].m_y == m - 1)
								{
									nextCurrentPiece = &m_pieces[x];
								}

								if ((board[n - 1][m - 1] == basic || board[n - 1][m - 1] == king))
								{
									if (m_pieces[x].m_x == n - 2 && m_pieces[x].m_y == m - 2)
									{
										nextCurrentPiece = &m_pieces[x];
									}
								}
								if ((board[n + 1][m - 1] == basic || board[n + 1][m - 1] == king))
								{
									if (m_pieces[x].m_x == n + 2 && m_pieces[x].m_y == m - 2)
									{
										nextCurrentPiece = &m_pieces[x];
									}
								}
								else //a king
								{

									if (m_pieces[x].m_x == n - 1 && m_pieces[x].m_y == m + 1 ||
										m_pieces[x].m_x == n + 1 && m_pieces[x].m_y == m + 1 ||
										m_pieces[x].m_x == n - 1 && m_pieces[x].m_y == m - 1 ||
										m_pieces[x].m_x == n + 1 && m_pieces[x].m_y == m - 1)
									{
										currentPiece = &m_pieces[x];
									}

									if ((board[n - 1][m - 1] == basic || board[n - 1][m - 1] == king))
									{
										if (m_pieces[x].m_x == n - 2 && m_pieces[x].m_y == m - 2)
										{
											nextCurrentPiece = &m_pieces[x];
										}
									}
									if ((board[n + 1][m - 1] == basic || board[n + 1][m - 1] == king))
									{
										if (m_pieces[x].m_x == n + 2 && m_pieces[x].m_y == m - 2)
										{
											nextCurrentPiece = &m_pieces[x];
										}
									}
									if ((board[n - 1][m + 1] == basic || board[n - 1][m + 1] == king))
									{
										if (m_pieces[x].m_x == n - 2 && m_pieces[x].m_y == m + 2)
										{
											nextCurrentPiece = &m_pieces[x];
										}
									}
									if ((board[n + 1][m + 1] == basic || board[n + 1][m + 1] == king))
									{
										if (m_pieces[x].m_x == n + 2 && m_pieces[x].m_y == m + 2)
										{
											nextCurrentPiece = &m_pieces[x];
										}
									}
								}
							}
						}
						PieceType nextCurrentPieceType;

						if (playerTurn)
						{
							if (nextCurrentPiece->m_type)
								nextCurrentPieceType = WHITEKING;
							else
								nextCurrentPieceType = WHITE;
						}
						else
						{
							if (nextCurrentPiece->m_type)
								nextCurrentPieceType = BLACKKING;
							else
								nextCurrentPieceType = BLACK;
						}

						//perform move
						clone.SimulateGame(glm::vec4(nextCurrentPiece->m_x, nextCurrentPiece->m_y, n, m), nextCurrentPieceType);

						clone.CheckWinner();

						playerTurn = !playerTurn;

						clone.RemoveValidMoves();

					}

					//if win +1 to value
					if (clone.GetWinState() == AI)
					{
						value++;
					}
					else//-1 from value
					{
						value--;
					}

					//delete clone

				}
				//push action score into vector
				actionScores.push_back(value);
			}
		}
	}

	float bestActionScore = -999.f;
	int bestAction;
	//return move with best actionscore
	for (int x = 0; x < actionScores.size(); ++x)
	{
		if (actionScores[x] > bestActionScore)
		{
			bestActionScore = actionScores[x];
			bestAction = x;
		}
	}

	Piece* currentPiece;

	//find move associated with this actionscore
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (board[i][j] == VALID)
			{
				if (bestAction > 0)
				{
					bestAction--;
				}
				else
				{
					for (int x = 0; x < 12; ++x)
					{
						if (m_pieces[x].m_type == false)
						{
							if (m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j - 1 ||
								m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j - 1)
							{
								currentPiece = &m_pieces[x];
								return &glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
							}

							if ((board[i - 1][j - 1] == WHITE || board[i - 1][j - 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j - 2)
								{
									currentPiece = &m_pieces[x];
									return &glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
								}
							}
							if ((board[i + 1][j - 1] == WHITE || board[i + 1][j - 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j - 2)
								{
									currentPiece = &m_pieces[x];
									return &glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
								}
							}
							else //a king
							{

								if (m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j + 1 ||
									m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j + 1 ||
									m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j - 1 ||
									m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j - 1)
								{
									currentPiece = &m_pieces[x];
									return &glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
								}

								if ((board[i - 1][j - 1] == WHITE || board[i - 1][j - 1] == WHITEKING))
								{
									if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j - 2)
									{
										currentPiece = &m_pieces[x];
										return &glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
									}
								}
								if ((board[i + 1][j - 1] == WHITE || board[i + 1][j - 1] == WHITEKING))
								{
									if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j - 2)
									{
										currentPiece = &m_pieces[x];
										return &glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
									}
								}
								if ((board[i - 1][j + 1] == WHITE || board[i - 1][j + 1] == WHITEKING))
								{
									if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j + 2)
									{
										currentPiece = &m_pieces[x];
										return &glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
									}
								}
								if ((board[i + 1][j + 1] == WHITE || board[i + 1][j + 1] == WHITEKING))
								{
									if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j + 2)
									{
										currentPiece = &m_pieces[x];
										return &glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
									}
								}
							}
						}
					}
				}

			}
		}
	}

	return &glm::vec4(0);
}

Piece* CheckerAI::GetPiece(int i)
{
	return &m_pieces[i];
}

Piece* CheckerAI::GetPieceAtPos(int x, int y)
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

void CheckerAI::SetBoardState(PieceType** currentState)
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

void CheckerAI::SetBoard(CheckerBoard* board)
{
	m_board = board;
}
