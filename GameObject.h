#pragma once

#include <vector>
#include <memory>

#include "Dependencies\glew\glew.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include "Dependencies\glm\gtc\quaternion.hpp"
#include "Dependencies\glm\gtx\quaternion.hpp"

#include "Shader.h"
#include "Texture2D.h"

#include "btBulletDynamicsCommon.h"

class GameObject
{
protected:
	// Render State
	GLuint VAO;
	GLuint VBO;
public:
	// Render State
	Shader shader;

	// Transformation
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 color;
	glm::quat rotation;

	// Render Info
	std::unique_ptr<btCollisionShape> shape;
	std::unique_ptr<btDefaultMotionState> motionState;
	std::shared_ptr<btRigidBody> rigidBody;

	std::string name;

	GameObject(Shader &shader);
	virtual ~GameObject();

	// Initializes and configures quad's buffer and vertex attributes
	virtual void initRenderData() {};
	// Draw functions
	virtual void DrawSprite() {};
	virtual void DrawSprite(glm::mat4 & model) {};
	virtual void DrawSprite(Texture2D & texture) {};
	virtual void DrawSprite(Texture2D & texture, glm::mat4 & model) {};
	// Init Rigidbody
	virtual void SetRigidBody(std::unique_ptr<btDiscreteDynamicsWorld>& dynamicsWorld, std::unique_ptr<btCollisionShape> &shape,
		std::string name,
		btVector3 &position = btVector3(0.0f, 0.0f, 0.0f), btQuaternion &rotation = btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
		btScalar mass = 0.0f, btVector3 &inertia = btVector3(0.0f, 0.0f, 0.0f));
};

class Box : public GameObject
{
public:
	Box(Shader &shader, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f)) : GameObject(shader) {
		this->shader = shader;
		this->initRenderData();
		this->position = position;
		this->scale = scale;
		this->color = color;
		this->rotation = rotation;
	};
	virtual ~Box() { GameObject::~GameObject(); };
	virtual void initRenderData() override;
	virtual void DrawSprite() override;
	virtual void DrawSprite(glm::mat4 & model) override;
	virtual void DrawSprite(Texture2D & texture) override;
	virtual void DrawSprite(Texture2D & texture, glm::mat4 & model) override;
	virtual void SetRigidBody(std::unique_ptr<btDiscreteDynamicsWorld>& dynamicsWorld, std::unique_ptr<btCollisionShape> &shape,
		std::string name,
		btVector3 &position = btVector3(0.0f, 0.0f, 0.0f), btQuaternion &rotation = btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
		btScalar mass = 0.0f, btVector3 &inertia = btVector3(0.0f, 0.0f, 0.0f))
	{
		GameObject::SetRigidBody(dynamicsWorld, shape, name, position, rotation, mass, inertia);
	}
};