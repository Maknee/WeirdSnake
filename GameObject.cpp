#include "GameObject.h"
#include "GraphicsEngine.h"
GameObject::GameObject(Shader & shader)
{
	this->shader = shader;
	this->initRenderData();
}

GameObject::~GameObject()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	motionState.reset();
	rigidBody.reset();
	shape.reset();
}

void GameObject::SetRigidBody(std::unique_ptr<btDiscreteDynamicsWorld>& dynamicsWorld, std::unique_ptr<btCollisionShape> &shape,
	std::string name,
	btVector3 &position, btQuaternion &rotation,
	btScalar mass, btVector3 &inertia)
{
	this->shape = std::move(shape);
	this->motionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(btTransform(rotation, position)));
	btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo(mass, this->motionState.get(), this->shape.get(), inertia);
	this->rigidBody = std::shared_ptr<btRigidBody>(new btRigidBody(rigidBodyConstructionInfo));
	this->name = name;
	this->rigidBody->setUserPointer(this);
	dynamicsWorld->addRigidBody(this->rigidBody.get());
};



void Box::initRenderData()
{
	// Configure VAO/EBO/VBO
	GLfloat vertices[] = {
		// Positions          // Normals           // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Box::DrawSprite()
{
	// Prepare transformations
	glm::mat4 model;
	model = glm::scale(model, scale);
	model = model * glm::toMat4(rotation);
	model = glm::translate(model, this->position);

	glm::mat4 view;
	view = glm::lookAt(GraphicsEngine::cameraPos,
		GraphicsEngine::cameraPos + GraphicsEngine::cameraFront,
		GraphicsEngine::cameraUp);

	glm::mat4 projection;
	projection = glm::perspective(GraphicsEngine::fov,
		static_cast<GLfloat>(GraphicsEngine::WIDTH) / static_cast<GLfloat>(GraphicsEngine::HEIGHT),
		0.1f, 100.0f);

	this->shader.Use();

	this->shader.SetMatrix4("model", model);
	this->shader.SetMatrix4("view", view);
	this->shader.SetMatrix4("projection", projection);

	this->shader.SetVector3f("objectColor", this->color);
	this->shader.SetVector3f("viewPos", GraphicsEngine::cameraPos);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Box::DrawSprite(glm::mat4 &model)
{
	// Prepare transformations
	glm::mat4 view;
	view = glm::lookAt(GraphicsEngine::cameraPos,
		GraphicsEngine::cameraPos + GraphicsEngine::cameraFront,
		GraphicsEngine::cameraUp);

	glm::mat4 projection;
	projection = glm::perspective(GraphicsEngine::fov,
		static_cast<GLfloat>(GraphicsEngine::WIDTH) / static_cast<GLfloat>(GraphicsEngine::HEIGHT),
		0.1f, 100.0f);

	this->shader.Use();

	this->shader.SetMatrix4("model", model);
	this->shader.SetMatrix4("view", view);
	this->shader.SetMatrix4("projection", projection);

	this->shader.SetVector3f("objectColor", this->color);
	this->shader.SetVector3f("viewPos", GraphicsEngine::cameraPos);

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}


void Box::DrawSprite(Texture2D &texture)
{
	// Prepare transformations
	glm::mat4 model;
	model = glm::scale(model, scale);
	model = model * glm::toMat4(rotation);
	model = glm::translate(model, this->position);
	
	glm::mat4 view;
	view = glm::lookAt(GraphicsEngine::cameraPos,
		GraphicsEngine::cameraPos + GraphicsEngine::cameraFront,
		GraphicsEngine::cameraUp);

	glm::mat4 projection;
	projection = glm::perspective(GraphicsEngine::fov,
		static_cast<GLfloat>(GraphicsEngine::WIDTH) / static_cast<GLfloat>(GraphicsEngine::HEIGHT),
		0.1f, 100.0f);

	this->shader.Use();

	this->shader.SetMatrix4("model", model);
	this->shader.SetMatrix4("view", view);
	this->shader.SetMatrix4("projection", projection);

	this->shader.SetVector3f("objectColor", this->color);
	this->shader.SetVector3f("viewPos", GraphicsEngine::cameraPos);

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();
	this->shader.SetTexture("bodyTexture", 0);

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Box::DrawSprite(Texture2D &texture, glm::mat4 &model)
{
	// Prepare transformations
	glm::mat4 view;
	view = glm::lookAt(GraphicsEngine::cameraPos,
		GraphicsEngine::cameraPos + GraphicsEngine::cameraFront,
		GraphicsEngine::cameraUp);

	glm::mat4 projection;
	projection = glm::perspective(GraphicsEngine::fov,
		static_cast<GLfloat>(GraphicsEngine::WIDTH) / static_cast<GLfloat>(GraphicsEngine::HEIGHT),
		0.1f, 100.0f);

	this->shader.Use();

	this->shader.SetMatrix4("model", model);
	this->shader.SetMatrix4("view", view);
	this->shader.SetMatrix4("projection", projection);

	this->shader.SetVector3f("objectColor", this->color);
	this->shader.SetVector3f("viewPos", GraphicsEngine::cameraPos);

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();
	this->shader.SetTexture("bodyTexture", 0);

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}