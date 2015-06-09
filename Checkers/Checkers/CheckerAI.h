#pragma once
#include "CheckerPiece.h"
#include "gl_core_4_4.h"
#include <include\GLFW\glfw3.h>
#include "glm\vec2.hpp"
#include "glm\vec4.hpp"

class CheckerBoard;

class CheckerAI
{
public:
	CheckerAI();
	~CheckerAI();

	void Update(float dt);

	Piece* GetPieceAtPos(int x, int y);
	Piece* GetPiece(int i);
	Piece* GetPieces();

	void SetBoard(CheckerBoard* board);

	void SetBoardState(PieceType** currentState);

	void Reset();

protected:
	Piece m_pieces[12];

	CheckerBoard* m_board;

	void MakeDecision();
};