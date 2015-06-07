#pragma once
#include "CheckerAI.h"
#include "CheckerPlayer.h"
#include "CheckerPiece.h"
#include "Gizmos.h"
#include "Application.h"
#include "FlyCamera.h"
#include "glm\vec2.hpp"

class CheckerBoard : public Application
{
public:
	CheckerBoard();
	~CheckerBoard();

	virtual void Create();
	virtual void Destroy();
	virtual void Update(float dt);
	virtual void Draw();

	void ClearBoard();

	void CheckValidMoves(glm::vec2 piece);

	PieceType** GetBoardState();
	GameState GetWinState();
	Piece* GetPlayerPieces();
	Piece* GetAIPieces();

	void FindAllPotentialMoves(bool white);
	void RemoveValidMoves();
	void CheckWinner();

	//
	void SetWinner(bool white);

	GameState SimulateGame(glm::vec4 move, PieceType piece);

	bool GetForceJump();

protected:
	FlyCamera m_camera;

	PieceType m_board[8][8];
	PieceType** m_validMoves;

	CheckerAI m_ai;
	CheckerPlayer m_player;

	GameState m_gameState;

	bool m_playerTurn;
	bool m_forceJump;
};