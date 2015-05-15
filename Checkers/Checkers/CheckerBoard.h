#pragma once
#include "CheckerAI.h"
#include "CheckerPlayer.h"
#include "Gizmos.h"
#include "Application.h"
#include "FlyCamera.h"

static enum PieceType {EMPTY, WHITE, BLACK, WHITEKING, BLACKKING};

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

protected:
	FlyCamera m_camera;

	PieceType m_board[8][8];

	CheckerAI* m_ai;
	CheckerPlayer* m_player;

	bool m_playerTurn;
};