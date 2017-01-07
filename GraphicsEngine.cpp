#include "GraphicsEngine.h"

// GLFW
GLFWwindow* GraphicsEngine::window;

// View vecs
glm::vec3 GraphicsEngine::cameraPos;
glm::vec3 GraphicsEngine::cameraFront;
glm::vec3 GraphicsEngine::cameraUp;

// Static variables
bool GraphicsEngine::keys[1024];
GLdouble GraphicsEngine::deltaTime;
GLdouble GraphicsEngine::lastFrame;

GLfloat GraphicsEngine::lastX;
GLfloat GraphicsEngine::lastY;
GLfloat GraphicsEngine::yaw;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat GraphicsEngine::pitch;

GLfloat GraphicsEngine::fov;

GraphicsEngine::GraphicsEngine()
{
	
}

GraphicsEngine::~GraphicsEngine()
{
	
	for (auto const & room : rooms)
	{
		for (auto const & tile : room)
		{
			dynamicsWorld->removeRigidBody(tile->rigidBody.get());
		}
	}

	for (auto const & snakePart : snake)
	{
		dynamicsWorld->removeRigidBody(snakePart->rigidBody.get());
	}
	
	
	/*
	for (int i = 0; i < this->dynamicsWorld->getNumCollisionObjects(); i++)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		//btRigidBody* body = btRigidBody::upcast(obj);
		dynamicsWorld->removeCollisionObject(obj);
	}
	*/
	
	//dynamicsWorld->removeRigidBody(fallRigidBody);
	glfwTerminate();
}

void GraphicsEngine::init()
{
	// static variables
	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	deltaTime = 0.0f;
	lastFrame = 0.0f;
	lastX = 400;
	lastY = 300;
	yaw = -90.0f;
	pitch = 0.0f;
	fov = 45.0f;

	ResourceManager::initPath();
	initWindow();
	initBulletPhysics();
}

void GraphicsEngine::initBulletPhysics()
{
	// Setup collision
	// Broadphase is use to detect things that should not collide
	broadphase = std::unique_ptr<btBroadphaseInterface>(new btDbvtBroadphase());
	// Collision configuration is used for full collision detection (i.e other algorithms such as btGImpactMeshShape)
	collisionConfiguration = std::unique_ptr<btDefaultCollisionConfiguration>(new btDefaultCollisionConfiguration());
	collisionDispatcher = std::unique_ptr<btCollisionDispatcher>(new btCollisionDispatcher(collisionConfiguration.get()));

	// Causes object to interact properly, (i.e gravity, forces, collisions, constraints, etc)
	solver = std::unique_ptr<btSequentialImpulseConstraintSolver>(new btSequentialImpulseConstraintSolver());

	// Create a world consisting of these collision configurations
	dynamicsWorld = std::unique_ptr<btDiscreteDynamicsWorld>(new btDiscreteDynamicsWorld(collisionDispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get()));

	// Set the gravity of the world
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
#if defined(_DEBUG_MODE)
	drawer.init();
	drawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints + btIDebugDraw::DBG_DrawAabb);
	dynamicsWorld->setDebugDrawer(&drawer);
#endif
	/*
	// Create Collision bodies for collisions only (no mass, inertia, etc...)
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btCollisionShape* fallShape = new btSphereShape(1);

	// Position collision bodies with rigid bodies. Motion States are used to render object from physics body
	// Quaternions - oritentiation, position 1 meter below ground
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	
	// Define how the rigidbody should act 
	// Mass - 0 means infinite mass, last - inertia
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));

	// Create a rigidbody from construction
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

	// Add the rigidbody to the world
	dynamicsWorld->addRigidBody(groundRigidBody);


	btDefaultMotionState* fallMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	dynamicsWorld->addRigidBody(fallRigidBody);


	for (int i = 0; i < 300; i++) {
		dynamicsWorld->stepSimulation(1 / 60.f, 10);

		btTransform trans;
		fallRigidBody->getMotionState()->getWorldTransform(trans);

		std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
	}

	dynamicsWorld->removeRigidBody(fallRigidBody);
	delete fallRigidBody->getMotionState();
	delete fallRigidBody;

	dynamicsWorld->removeRigidBody(groundRigidBody);
	delete groundRigidBody->getMotionState();
	delete groundRigidBody;


	delete fallShape;

	delete groundShape;
	*/
}

glm::vec3 btToGlm(const btVector3 & vector)
{
	return glm::vec3(vector.getX(), vector.getY(), vector.getZ());
}

btVector3 glmToBt(const glm::vec3 & vector)
{
	return btVector3(vector.x, vector.y, vector.z);
}

btQuaternion glmToBt(const glm::quat & quat)
{
	return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

void GraphicsEngine::createRoom()
{
	std::vector<std::shared_ptr<GameObject>> mainRoom;
	for (GLfloat x = -5; x <= 5; x++)
	{
		for (GLfloat z = -5; z <= 5; z++)
		{
			std::shared_ptr<GameObject> floorTile(new Box(ResourceManager::GetShader("snake"),
				glm::vec3(x, 0.0f, z), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.8f, 0.85f, 0.5f),
				glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 0.0f))));

			std::unique_ptr<btCollisionShape> floorShape(new btBoxShape(glmToBt(floorTile->scale * glm::vec3(0.5f, 0.5f, 0.5f))));
			btScalar boxMass = 0.0f;
			btVector3 boxInertia(0.0f, 0.0f, 0.0f);
			//floorShape->calculateLocalInertia(boxMass, boxInertia);
			floorTile->SetRigidBody(dynamicsWorld, floorShape, "floor",
				glmToBt(floorTile->position), glmToBt(floorTile->rotation),
				boxMass, boxInertia);
			
			// Bounciness - higher values = higher bounce
			floorTile->rigidBody->setRestitution(0.00f);
			// Friction
			floorTile->rigidBody->setFriction(0.5f);

			mainRoom.push_back(std::move(floorTile));
		}
	}
	rooms.push_back(mainRoom);
}

void GraphicsEngine::initGameObjects()
{
	ResourceManager::LoadShader("Shaders\\SnakeBodyShader.vs", "Shaders\\SnakeBodyShader.frag", nullptr, "snake");
	//GameObject* box = new Box(ResourceManager::GetShader("snake"));
	std::unique_ptr<GameObject> box(new Box(ResourceManager::GetShader("snake"),
		glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.8f, 0.85f, 0.5f), 
		glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 0.0f))));

	std::unique_ptr<btCollisionShape> boxShape(new btBoxShape(glmToBt(box->scale * glm::vec3(0.5f, 0.5f, 0.5f))));
	btScalar boxMass = 1.2f;
	btVector3 boxInertia(1.0f, 1.0f, 1.0f);
	boxShape->calculateLocalInertia(boxMass, boxInertia);
	box->SetRigidBody(dynamicsWorld, boxShape, "box",
		glmToBt(box->position), glmToBt(box->rotation), 
		boxMass, boxInertia);

	box->rigidBody->setRestitution(0.00f);

	box->rigidBody->setFriction(0.5f);

	box->rigidBody->setDamping(0.1f, 0.2f);

	//std::cout << box->motionState->m_graphicsWorldTrans.getOrigin().getY();
	snake.push_back(std::move(box));

	createRoom();

	ResourceManager::LoadTexture("Textures\\container.jpg", GL_FALSE, "c");
}

void GraphicsEngine::run()
{
	initGameObjects();
	glEnable(GL_DEPTH_TEST);
	
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		processInput();
		performMovement();
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if defined(_DEBUG_MODE)
		dynamicsWorld->debugDrawWorld();
#endif

		dynamicsWorld->stepSimulation(1 / 60.f, 10);

		int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
			btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
			btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());

			GameObject* obAUserPointer = static_cast<GameObject*>(obA->getUserPointer());
			if (obAUserPointer != nullptr)
			{
				if (obAUserPointer->name.compare("box") == 0)
				{
					//btRigidBody::upcast(obA)->activate();
					btRigidBody::upcast(obA)->applyCentralForce(btVector3(0.0f, 2.0f, 0.0f));
				}
			}
			GameObject* obBUserPointer = static_cast<GameObject*>(obB->getUserPointer());
			if (obBUserPointer != nullptr)
			{
				std::cout << obBUserPointer->name << std::endl;
				if (obAUserPointer->name.compare("box") == 0)
				{
					//exit(0);
				}
			}
			/*
			if (obA == snake[0]->rigidBody.get())
			{
				obA->activate();
				snake[0]->rigidBody.get()->applyCentralImpulse(btVector3(0.0f, 0.0f, -0.1f));
			}
			
			/*
			int numContacts = contactManifold->getNumContacts();
			for (int j = 0; j < numContacts; j++)
			{
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.getDistance() < 0.f)
				{
					const btVector3& ptA = pt.getPositionWorldOnA();
					const btVector3& ptB = pt.getPositionWorldOnB();
					const btVector3& normalOnB = pt.m_normalWorldOnB;
				}
			}
			*/
		}

		btTransform trans;
		snake[0]->rigidBody->getMotionState()->getWorldTransform(trans);
		//std::cout << dynamicsWorld->getNumCollisionObjects() << "sphere height: " << trans.getOrigin().getY() << std::endl;

		// Render
		for (auto const & room : rooms)
		{
			for (auto const & tile : room)
			{
				tile->shader.Use();
				tile->shader.SetVector3f("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
				tile->shader.SetVector3f("lightPos", glm::vec3(5.0f * glm::cos(glfwGetTime() * 1.0f), 5.0f, 5.0f * glm::sin(glfwGetTime() * 1.0f)));
				
				btTransform trans;
				tile->rigidBody->getMotionState()->getWorldTransform(trans);
				glm::mat4 model;
				trans.getOpenGLMatrix(glm::value_ptr(model));

				tile->DrawSprite(model);
			}
		}

		for (auto const & snakePart : snake)
		{
			snakePart->shader.Use();
			snakePart->shader.SetVector3f("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
			snakePart->shader.SetVector3f("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));

			btTransform trans;
			snakePart->rigidBody->getMotionState()->getWorldTransform(trans);
			glm::mat4 model;

			trans.getOpenGLMatrix(glm::value_ptr(model));
			std::cout << trans.getOrigin().getY() << std::endl;
			snakePart->DrawSprite(ResourceManager::GetTexture("c"), model);

		}
		// Swap buffers
		glfwSwapBuffers(window);
	}

}

void GraphicsEngine::processInput()
{
	GLdouble currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	static GLuint numFrames = 0;
	static GLdouble previousFrame = glfwGetTime();
	numFrames++;
	if (currentFrame - previousFrame >= 1.0)
	{
		std::stringstream ss;
		ss << "Frames per second: " << static_cast<GLdouble>(numFrames) / (currentFrame - previousFrame) <<
			" Seconds per Frame0: " << static_cast<GLdouble>(1000.0) / static_cast<GLdouble>(numFrames) << " ms";

		glfwSetWindowTitle(window, ss.str().c_str());
		
		numFrames = 0;
		previousFrame = currentFrame;
	}

	GLfloat cameraSpeed = 5.0f * deltaTime;

	if (keys[GLFW_KEY_W] == true)
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S] == true)
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A] == true)
		cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
	if (keys[GLFW_KEY_D] == true)
		cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
	if (keys[GLFW_KEY_ESCAPE] == true)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (keys[GLFW_KEY_UP] == true)
	{
		previousKey = KEY_UP;
	}
	if (keys[GLFW_KEY_LEFT] == true)
	{
		previousKey = KEY_LEFT;
	}
	if (keys[GLFW_KEY_RIGHT] == true)
	{
		previousKey = KEY_RIGHT;
	}
	if (keys[GLFW_KEY_DOWN] == true)
	{
		previousKey = KEY_DOWN;
	}
}

void GraphicsEngine::performMovement()
{

	switch (previousKey)
	{
	case KEY_NONE:
		break;
	case KEY_UP:
		snake[0]->position.z -= 1.0f / 60;
		snake[0]->rigidBody->activate();
		snake[0]->rigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, -1.0f));
		//snake[0]->rigidBody->applyCentralForce(btVector3(0.0f, 0.0f, -0.1f));
		//snake[0]->rigidBody->applyCentralImpulse(btVector3(0.0f, 0.0f, -0.1f));
		break;
	case KEY_LEFT:
		snake[0]->position.x -= 1.0f / 60;
		snake[0]->rigidBody->activate();
		snake[0]->rigidBody->setLinearVelocity(btVector3(-1.0f, 0.0f, 0.0f));
		//snake[0]->rigidBody->applyCentralImpulse(btVector3(-0.1f, 0.0f, 0.0f));
		break;
	case KEY_RIGHT:
		snake[0]->position.x += 1.0f / 60;
		snake[0]->rigidBody->activate();
		snake[0]->rigidBody->setLinearVelocity(btVector3(1.0f, 0.0f, 0.0f));
		//snake[0]->rigidBody->applyCentralImpulse(btVector3(0.1f, 0.0f, 0.0f));
		break;
	case KEY_DOWN:
	{
		snake[0]->position.z += 1.0f / 60;
		snake[0]->rigidBody->activate();
		snake[0]->rigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
		btTransform tr;
		tr = snake[0]->rigidBody->getCenterOfMassTransform();
		btQuaternion quat;
		quat.setEuler(0.0f, 0.0f, 0.0f);
		tr.setRotation(quat);
		snake[0]->rigidBody->setCenterOfMassTransform(tr);
		snake[0]->rigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 1.0f));
		//snake[0]->rigidBody->setGravity(btVector3(0.0f, -10.0f, 0.0f));
		//snake[0]->rigidBody->applyCentralImpulse(btVector3(0.0f, 0.0f, 0.1f));
		break;
	}
	default:
		break;
	}

	/*
	GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
	static GLfloat lastFrameInterval = 0.0;
	if (currentFrame - lastFrameInterval > deltaTime)
	{
		if (!snake.empty())
		{
			for (int i = snake.size() - 1; i > 0; --i)
			{
				snake[i]->position = snake[i - 1]->position;
			}
		}
		switch (previousKey)
		{
		case KEY_NONE:
			break;
		case KEY_UP:
			snake[0]->position.z -= 1.0f /60;
			break;
		case KEY_LEFT:
			snake[0]->position.x -= 1.0f /60;
			break;
		case KEY_RIGHT:
			snake[0]->position.x += 1.0f /60;
			break;
		case KEY_DOWN:
			snake[0]->position.z += 1.0f/ 60;
			break;
		default:
			break;
		}
		lastFrameInterval = currentFrame;
	}
	*/
}

void GraphicsEngine::initWindow()
{
	/*
#if defined(WIN32)
	FreeConsole();
#endif
	*/
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);
}

// Is called whenever a key is pressed/released via GLFW
void GraphicsEngine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

bool firstMouse = true;

void GraphicsEngine::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = static_cast<GLfloat>(xpos);
		lastY = static_cast<GLfloat>(ypos);
		firstMouse = false;
	}
	GLfloat xoffset = static_cast<GLfloat>(xpos) - lastX;
	GLfloat yoffset = lastY - static_cast<GLfloat>(ypos); // Reversed since y-coordinates range from bottom to top
	lastX = static_cast<GLfloat>(xpos);
	lastY = static_cast<GLfloat>(ypos);

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}

void GraphicsEngine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= static_cast<GLfloat>(yoffset) * 0.05f;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}