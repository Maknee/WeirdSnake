#pragma once

#include <Windows.h>
#include <iostream>
#include <memory>
#include <map>
#include <string>

// Other includes
#include "Shader.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "DebugDrawer.h"

// GLFW
#include "Dependencies\glfw3\glfw3.h"
#include "SOIL.h"

// GLEW
#define GLEW_STATIC
#include "Dependencies\glew\glew.h"

// GLM
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"

#include "btBulletDynamicsCommon.h"

// DEFINE
#define _DEBUG_MODEw

class GraphicsEngine
{
private:
	// Physics
	std::unique_ptr<btBroadphaseInterface> broadphase;
	std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> collisionDispatcher;
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
	
	DebugDrawer drawer;
	// Graphics
	Shader boxShader;
	Shader lightShader;

	enum KeyPress {
		KEY_NONE,
		KEY_UP,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_DOWN
	}previousKey = KEY_NONE;

	// GameObjects
	//std::map<std::string, GameObject> gameObjects;

	std::vector<std::unique_ptr<GameObject>> snake;
	std::vector<std::vector<std::shared_ptr<GameObject>>> rooms;
	// GLFW
	static GLFWwindow* window;

	// Static variables
	static bool keys[1024];
	static GLdouble deltaTime;
	static GLdouble lastFrame;

	static GLfloat lastX, lastY;
	static GLfloat yaw;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
	static GLfloat pitch;

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	
	void processInput();
	void performMovement();
	// Initialization windows functions
	void initWindow();
	void initBulletPhysics();
	void initGameObjects();

	// Create Shapes Functions
	void createRoom();
public:
	static const GLuint WIDTH = 800;
	static const GLuint HEIGHT = 600;

	// View vecs
	static glm::vec3 cameraPos;
	static glm::vec3 cameraFront;
	static glm::vec3 cameraUp;

	// fov
	static GLfloat fov;

	GraphicsEngine();
	~GraphicsEngine();
	void init();
	void run();
};