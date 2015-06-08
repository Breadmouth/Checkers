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
	bool GetPlayerTurn();
	bool GetMoveMade();
	//true means multiple jumper exists, false means otherwise
	bool GetMultipleJumper();

	void FindAllPotentialMoves(bool white);
	void RemoveValidMoves();
	void CheckWinner();

	//
	void SetMultipleJumper(Piece* multipleJumper);
	void SetWinner(bool white);
	void SetMoveMade(bool moveMade);

	GameState SimulateGame(glm::vec4 move, PieceType piece);

	void SwitchTurn();

	bool GetForceJump();

protected:
	FlyCamera m_camera;

	PieceType m_board[8][8];
	PieceType** m_validMoves;

	CheckerAI m_ai;
	CheckerPlayer m_player;

	GameState m_gameState;

	Piece* m_multipleJumper;

	bool m_playerTurn;
	bool m_moveMade;
	bool m_forceJump;
};