#pragma once
#include "CheckerPiece.h"
#include "gl_core_4_4.h"
#include <include\GLFW\glfw3.h>
#include "glm\vec2.hpp"
#include "glm\vec4.hpp"

class CheckerPlayer
{
public:
	CheckerPlayer();
	~CheckerPlayer();

	glm::vec4* Update(float dt);

	void SetWindow(GLFWwindow* window);

	Piece* GetPieceAtPos(int x, int y);
	Piece* GetPiece(int i);

	glm::vec2* GetCursorPos();

protected:
	Piece m_pieces[12];
	Piece* m_currentPiece;

	glm::vec2 m_cursor;

	float m_selectCooldown;

	GLFWwindow* m_window;
};