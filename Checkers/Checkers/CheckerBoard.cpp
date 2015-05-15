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

	if (m_playerTurn)
	{
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

void CheckerBoard::Destroy()
{

}

void CheckerBoard::ClearBoard()
{

}

