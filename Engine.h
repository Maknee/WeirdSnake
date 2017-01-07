#pragma once

// Other includes
#include "GraphicsEngine.h"
#include "SoundEngine.h"
#include "ResourceManager.h"

class Engine
{
private:
	SoundEngine soundEngine;
	GraphicsEngine graphicsEngine;
public:
	Engine();
	~Engine();
	void init();
	void run();
};