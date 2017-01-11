#pragma once

// Other includes
#include "GraphicsEngine.h"
#include "SoundEngine.h"
#include "ResourceManager.h"

class Engine
{
private:
public:
	static Engine* thisEngine;
	GraphicsEngine graphicsEngine;
	SoundEngine soundEngine;

	Engine();
	~Engine();
	void init();
	void run();
};