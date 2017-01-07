#include "DebugDrawer.h"
#include "GraphicsEngine.h"

#include <iostream> //printf debugging

DebugDrawer::DebugDrawer()
	:m_debugMode(0)
{
	
}

DebugDrawer::~DebugDrawer()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VAO);
}

void DebugDrawer::init()
{
	ResourceManager::LoadShader("Shaders\\DebugDraw.vs", "Shaders\\DebugDraw.frag", nullptr, "DebugDraw");
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
}

void    DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	if (m_debugMode > 0)
	{
		GLfloat vertices[] = 
		{ 
			from.getX(), from.getY(), from.getZ(),
			to.getX(), to.getY(), to.getZ()
		};
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(VAO);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Draw
		glm::mat4 model;

		glm::mat4 view;
		view = glm::lookAt(GraphicsEngine::cameraPos,
			GraphicsEngine::cameraPos + GraphicsEngine::cameraFront,
			GraphicsEngine::cameraUp);

		glm::mat4 projection;
		projection = glm::perspective(GraphicsEngine::fov,
			static_cast<GLfloat>(GraphicsEngine::WIDTH) / static_cast<GLfloat>(GraphicsEngine::HEIGHT),
			0.1f, 100.0f);
		
		Shader shader = ResourceManager::GetShader("DebugDraw");
		
		shader.Use();

		shader.SetMatrix4("model", model);
		shader.SetMatrix4("view", view);
		shader.SetMatrix4("projection", projection);
		shader.SetVector3f("Color", color.getX(), color.getY(), color.getZ());

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 3);
		glBindVertexArray(0);

		/*
		std::cout << "EYY";
		glPushMatrix();
		{
			glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);
			glVertexPointer(3,
				GL_FLOAT,
				0,
				&tmp);

			glPointSize(5.0f);
			glDrawArrays(GL_POINTS, 0, 2);
			glDrawArrays(GL_LINES, 0, 2);
		}
		glPopMatrix();
		*/
	}
}

void    DebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

void    DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	//glRasterPos3f(location.x(),  location.y(),  location.z());
	//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),textString);
}

void    DebugDrawer::reportErrorWarning(const char* warningString)
{
	printf(warningString);
}

void    DebugDrawer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	{
		//btVector3 to=pointOnB+normalOnB*distance;
		//const btVector3&from = pointOnB;
		//glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);   

		//GLDebugDrawer::drawLine(from, to, color);

		//glRasterPos3f(from.x(),  from.y(),  from.z());
		//char buf[12];
		//sprintf(buf," %d",lifeTime);
		//BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);
	}
}