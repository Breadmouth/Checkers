#pragma once
#include "CheckerPiece.h"
#include "gl_core_4_4.h"
#include <include\GLFW\glfw3.h>
#include "glm\vec2.hpp"
#include "glm\vec4.hpp"

class CheckerBoard;

class CheckerPlayer
{
public:
	CheckerPlayer();
	~CheckerPlayer();

	void Update(float dt);

	void SetWindow(GLFWwindow* window);
	void SetBoard(CheckerBoard* board);

	Piece* GetPieceAtPos(int x, int y);
	Piece* GetPiece(int i);
	Piece* GetPieces();

	glm::vec2* GetCursorPos();

	void SetBoardState(PieceType** currentState);

	void Reset();

protected:
	Piece m_pieces[12];
	Piece* m_currentPiece;

	glm::vec2 m_cursor;

	CheckerBoard* m_board;

	float m_selectCooldown;

	GLFWwindow* m_window;
};