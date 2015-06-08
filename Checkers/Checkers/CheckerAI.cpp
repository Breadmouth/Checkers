#include "CheckerAI.h"
#include "CheckerBoard.h"

#include <vector>
#include <time.h>
#include <cstdlib>

CheckerAI::CheckerAI()
{
	srand(time(NULL));

	bool second = false;
	int k = 0;
	for (int i = 0; i < 3; ++i)
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
}

CheckerAI::~CheckerAI()
{

}

void CheckerAI::Update(float dt)
{
	PieceType** board = m_board->GetBoardState();
	/////////////////////////////////////////////////////////////////////stil possible infinite loop + crash via memory leak
	/////////////////////////////////////////////////////////AI may not be considering double jumps properly

	bool validMove = false;
	for (int n = 0; n < 8; ++n)
	{
		for (int m = 0; m < 8; ++m)
		{
			if (board[n][m] == VALID)
				validMove = true;
		}
	}
	if (!validMove)
	{
		return;
	}

	std::vector<float> actionScores;

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (board[i][j] == VALID)
			{
				int value = 0;

				Piece* currentPiece = &m_pieces[0];
				int currentPiecePos;

				//find piece associated with move
				for (int x = 0; x < 12; ++x)
				{
					if (m_pieces[x].m_type == false)
					{
						if (((m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j - 1) ||
							(m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j - 1)) &&
							m_pieces[x].m_alive == true)
						{
							currentPiecePos = x;
						}

						if (i > 0 && j > 0 && (board[i - 1][j - 1] == WHITE || board[i - 1][j - 1] == WHITEKING))
						{
							if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j - 2 &&
								m_pieces[x].m_alive == true)
							{
								currentPiecePos = x;
							}
						}
						if (i < 7 && j > 0 && (board[i + 1][j - 1] == WHITE || board[i + 1][j - 1] == WHITEKING))
						{
							if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j - 2 &&
								m_pieces[x].m_alive == true)
							{
								currentPiecePos = x;
							}
						}
					}
					else //a king
					{

						if (((m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j + 1) ||
							(m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j + 1) ||
							(m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j - 1) ||
							(m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j - 1)) &&
							m_pieces[x].m_alive == true)
						{
							currentPiecePos = x;
						}

						if (i > 0 && j > 0 && (board[i - 1][j - 1] == WHITE || board[i - 1][j - 1] == WHITEKING))
						{
							if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j - 2 &&
								m_pieces[x].m_alive == true)
							{
								currentPiecePos = x;
							}
						}
						if (i < 7 && j > 0 && (board[i + 1][j - 1] == WHITE || board[i + 1][j - 1] == WHITEKING))
						{
							if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j - 2 &&
								m_pieces[x].m_alive == true)
							{
								currentPiecePos = x;
							}
						}
						if (i > 0 && j < 7 && (board[i - 1][j + 1] == WHITE || board[i - 1][j + 1] == WHITEKING))
						{
							if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j + 2 &&
								m_pieces[x].m_alive == true)
							{
								currentPiecePos = x;
							}
						}
						if (i < 7 && j < 7 && (board[i + 1][j + 1] == WHITE || board[i + 1][j + 1] == WHITEKING))
						{
							if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j + 2 &&
								m_pieces[x].m_alive == true)
							{
								currentPiecePos = x;
							}
						}
					}
				}

				for (int playout = 0; playout < 50; ++playout)
				{
					//clone the game
					CheckerBoard clone = *m_board;

					currentPiece = &clone.GetAIPieces()[currentPiecePos];

					if (j == 7)
					{
						currentPiece->m_type == true;
					}

					PieceType currentPieceType;
					if (currentPiece->m_type)
						currentPieceType = BLACKKING;
					else
						currentPieceType = BLACK;

					if (currentPiece->m_x - i > 1 ||
						currentPiece->m_y - i < -1)
					{
						clone.SetMultipleJumper(currentPiece);
					}

					//perform action
					clone.SimulateGame(glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j), currentPieceType);
					currentPiece->m_x = i;
					currentPiece->m_y = j;
					currentPiece->m_alive = true;

					clone.SetMoveMade(true);

					clone.RemoveValidMoves();

					bool playerTurn = clone.GetPlayerTurn();

					Piece* clonedPieces;

					//while no winner simulate rest of game
					while (clone.GetWinState() == UNKNOWN)
					{
						if (!clone.GetMultipleJumper() && clone.GetMoveMade())
						{
							clone.SwitchTurn();
							clone.SetMoveMade(false);
						}

						playerTurn = clone.GetPlayerTurn();

						clone.FindAllPotentialMoves(playerTurn);

						if (!clone.GetMultipleJumper() && clone.GetMoveMade())
						{
							continue;
						}

						PieceType** cloneBoard = clone.GetBoardState();

						//check if there is a valid move otherwise end game in !currentplayers favour
						bool boardValid = false;
						for (int n = 0; n < 8; ++n)
						{
							for (int m = 0; m < 8; ++m)
							{
								if (cloneBoard[n][m] == VALID)
									boardValid = true;
							}
						}
						if (!boardValid)
						{
							clone.SetWinner(!playerTurn);
							continue;
						}

						boardValid = false;
						int n;
						int m;
						while (!boardValid) //choose random valid move
						{
							n = rand() % 8;
							m = rand() % 8;

							if (cloneBoard[n][m] == VALID)
								boardValid = true;
						}

						PieceType basic;
						PieceType king;
						int dir;

						if (playerTurn)
						{
							basic = BLACK;
							king = BLACKKING;
							clonedPieces = clone.GetPlayerPieces();
							dir = 1;
						}
						else
						{
							basic = WHITE;
							king = WHITEKING;
							clonedPieces = clone.GetAIPieces();
							dir = -1;
						}

						Piece* nextCurrentPiece = &clonedPieces[0];

						//find piece associated with move
						for (int x = 0; x < 12; ++x)
						{
							if (clonedPieces[x].m_type == false)
							{
								if (((clonedPieces[x].m_x == n - 1 && clonedPieces[x].m_y == m + dir) ||
									(clonedPieces[x].m_x == n + 1 && clonedPieces[x].m_y == m + dir)) &&
									clonedPieces[x].m_alive == true)
								{
									nextCurrentPiece = &clonedPieces[x];
								}
								if (n > 0 && m > 0 && (cloneBoard[n - 1][m + dir] == basic || cloneBoard[n - 1][m + dir] == king))
								{
									if (clonedPieces[x].m_x == n - 2 && clonedPieces[x].m_y == m + dir + dir &&
										clonedPieces[x].m_alive == true)
									{
										nextCurrentPiece = &clonedPieces[x];
									}
								}
								if (n < 7 && m > 0 && (cloneBoard[n + 1][m + dir] == basic || cloneBoard[n + 1][m + dir] == king))
								{
									if (clonedPieces[x].m_x == n + 2 && clonedPieces[x].m_y == m + dir + dir &&
										clonedPieces[x].m_alive == true)
									{
										nextCurrentPiece = &clonedPieces[x];
									}
								}
							}
							else //a king
							{

								if (((clonedPieces[x].m_x == n - 1 && clonedPieces[x].m_y == m + 1) ||
									(clonedPieces[x].m_x == n + 1 && clonedPieces[x].m_y == m + 1) ||
									(clonedPieces[x].m_x == n - 1 && clonedPieces[x].m_y == m - 1) ||
									(clonedPieces[x].m_x == n + 1 && clonedPieces[x].m_y == m - 1)) &&
									clonedPieces[x].m_alive == true)
								{
									nextCurrentPiece = &clonedPieces[x];
								}

								if (n > 0 && m > 0 && (cloneBoard[n - 1][m - 1] == basic || cloneBoard[n - 1][m - 1] == king))
								{
									if (clonedPieces[x].m_x == n - 2 && clonedPieces[x].m_y == m - 2 &&
										clonedPieces[x].m_alive == true)
									{
										nextCurrentPiece = &clonedPieces[x];
									}
								}
								if (n < 7 && m > 0 && (cloneBoard[n + 1][m - 1] == basic || cloneBoard[n + 1][m - 1] == king))
								{
									if (clonedPieces[x].m_x == n + 2 && clonedPieces[x].m_y == m - 2 &&
										clonedPieces[x].m_alive == true)
									{
										nextCurrentPiece = &clonedPieces[x];
									}
								}
								if (n > 0 && m < 7 && (cloneBoard[n - 1][m + 1] == basic || cloneBoard[n - 1][m + 1] == king))
								{
									if (clonedPieces[x].m_x == n - 2 && clonedPieces[x].m_y == m + 2 &&
										clonedPieces[x].m_alive == true)
									{
										nextCurrentPiece = &clonedPieces[x];
									}
								}
								if (n < 7 && m < 7 && (cloneBoard[n + 1][m + 1] == basic || cloneBoard[n + 1][m + 1] == king))
								{
									if (clonedPieces[x].m_x == n + 2 && clonedPieces[x].m_y == m + 2 &&
										clonedPieces[x].m_alive == true)
									{
										nextCurrentPiece = &clonedPieces[x];
									}
								}
							}
						}

						if (m == 7 || m == 0)
						{
							nextCurrentPiece->m_type = true;
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

						if (nextCurrentPiece->m_x - n < -1 ||
							nextCurrentPiece->m_y - n > 1)
						{
							clone.SetMultipleJumper(nextCurrentPiece);
						}

						//perform move
						clone.SimulateGame(glm::vec4(nextCurrentPiece->m_x, nextCurrentPiece->m_y, n, m), nextCurrentPieceType);
						nextCurrentPiece->m_x = n;
						nextCurrentPiece->m_y = m;
						nextCurrentPiece->m_alive = true;

						clone.SetMoveMade(true);

						clone.RemoveValidMoves();

						//clean up memory
						for (int n = 0; n < 8; ++n)
						{
							delete cloneBoard[n];
						}
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
							if (((m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j - 1) ||
								(m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j - 1)) &&
								m_pieces[x].m_alive == true)
							{
								currentPiece = &m_pieces[x];
								if (j == 7 || j == 0)
								{
									currentPiece->m_type = true;
								}
								PieceType currentPieceType;
								if (currentPiece->m_type)
									currentPieceType = BLACKKING;
								else
									currentPieceType = BLACK;
								glm::vec4 move = glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
								currentPiece->m_x = i;
								currentPiece->m_y = j;
								m_board->SimulateGame(move, currentPieceType);

								m_board->SetMoveMade(true);
								return;
							}

							if (i > 0 && j > 0 && (board[i - 1][j - 1] == WHITE || board[i - 1][j - 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j - 2 &&
									m_pieces[x].m_alive == true)
								{
									currentPiece = &m_pieces[x];
									if (j == 7 || j == 0)
									{
										currentPiece->m_type = true;
									}
									PieceType currentPieceType;
									if (currentPiece->m_type)
										currentPieceType = BLACKKING;
									else
										currentPieceType = BLACK;
									glm::vec4 move = glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
									currentPiece->m_x = i;
									currentPiece->m_y = j;
									m_board->SimulateGame(move, currentPieceType);

									m_board->SetMultipleJumper(currentPiece);

									m_board->SetMoveMade(true);
									return;
								}
							}
							if (i < 7 && j > 0 && (board[i + 1][j - 1] == WHITE || board[i + 1][j - 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j - 2 &&
									m_pieces[x].m_alive == true)
								{
									currentPiece = &m_pieces[x];
									if (j == 7 || j == 0)
									{
										currentPiece->m_type = true;
									}
									PieceType currentPieceType;
									if (currentPiece->m_type)
										currentPieceType = BLACKKING;
									else
										currentPieceType = BLACK;
									glm::vec4 move = glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
									currentPiece->m_x = i;
									currentPiece->m_y = j;
									m_board->SimulateGame(move, currentPieceType);

									m_board->SetMultipleJumper(currentPiece);

									m_board->SetMoveMade(true);
									return;
								}
							}
						}
						else //a king
						{

							if (((m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j + 1) ||
								(m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j + 1) ||
								(m_pieces[x].m_x == i - 1 && m_pieces[x].m_y == j - 1) ||
								(m_pieces[x].m_x == i + 1 && m_pieces[x].m_y == j - 1)) &&
								m_pieces[x].m_alive == true)
							{
								currentPiece = &m_pieces[x];
								if (j == 7 || j == 0)
								{
									currentPiece->m_type = true;
								}
								PieceType currentPieceType;
								if (currentPiece->m_type)
									currentPieceType = BLACKKING;
								else
									currentPieceType = BLACK;
								glm::vec4 move = glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
								currentPiece->m_x = i;
								currentPiece->m_y = j;
								m_board->SimulateGame(move, currentPieceType);

								m_board->SetMoveMade(true);
								return;
							}

							if (i > 0 && j > 0 && (board[i - 1][j - 1] == WHITE || board[i - 1][j - 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j - 2 &&
									m_pieces[x].m_alive == true)
								{
									currentPiece = &m_pieces[x];
									if (j == 7 || j == 0)
									{
										currentPiece->m_type = true;
									}
									PieceType currentPieceType;
									if (currentPiece->m_type)
										currentPieceType = BLACKKING;
									else
										currentPieceType = BLACK;
									glm::vec4 move = glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
									currentPiece->m_x = i;
									currentPiece->m_y = j;
									m_board->SimulateGame(move, currentPieceType);

									m_board->SetMultipleJumper(currentPiece);

									m_board->SetMoveMade(true);
									return;
								}
							}
							if (i < 7 && j > 0 && (board[i + 1][j - 1] == WHITE || board[i + 1][j - 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j - 2 &&
									m_pieces[x].m_alive == true)
								{
									currentPiece = &m_pieces[x];
									if (j == 7 || j == 0)
									{
										currentPiece->m_type = true;
									}
									PieceType currentPieceType;
									if (currentPiece->m_type)
										currentPieceType = BLACKKING;
									else
										currentPieceType = BLACK;
									glm::vec4 move = glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
									currentPiece->m_x = i;
									currentPiece->m_y = j;
									m_board->SimulateGame(move, currentPieceType);

									m_board->SetMultipleJumper(currentPiece);

									m_board->SetMoveMade(true);
									return;
								}
							}
							if (i > 0 && j < 7 && (board[i - 1][j + 1] == WHITE || board[i - 1][j + 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i - 2 && m_pieces[x].m_y == j + 2 &&
									m_pieces[x].m_alive == true)
								{
									currentPiece = &m_pieces[x];
									if (j == 7 || j == 0)
									{
										currentPiece->m_type = true;
									}
									PieceType currentPieceType;
									if (currentPiece->m_type)
										currentPieceType = BLACKKING;
									else
										currentPieceType = BLACK;
									glm::vec4 move = glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
									currentPiece->m_x = i;
									currentPiece->m_y = j;
									m_board->SimulateGame(move, currentPieceType);

									m_board->SetMultipleJumper(currentPiece);

									m_board->SetMoveMade(true);
									return;
								}
							}
							if (i < 7 && j < 7 && (board[i + 1][j + 1] == WHITE || board[i + 1][j + 1] == WHITEKING))
							{
								if (m_pieces[x].m_x == i + 2 && m_pieces[x].m_y == j + 2 &&
									m_pieces[x].m_alive == true)
								{
									currentPiece = &m_pieces[x];
									if (j == 7 || j == 0)
									{
										currentPiece->m_type = true;
									}
									PieceType currentPieceType;
									if (currentPiece->m_type)
										currentPieceType = BLACKKING;
									else
										currentPieceType = BLACK;
									glm::vec4 move = glm::vec4(currentPiece->m_x, currentPiece->m_y, i, j);
									currentPiece->m_x = i;
									currentPiece->m_y = j;
									m_board->SimulateGame(move, currentPieceType);

									m_board->SetMultipleJumper(currentPiece);

									m_board->SetMoveMade(true);
									return;
								}
							}
						}
					}
				}

			}
		}
	}

	return;
}

Piece* CheckerAI::GetPieces()
{
	return &m_pieces[0];
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
