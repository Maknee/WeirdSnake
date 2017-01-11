#include "Engine.h"

Engine* Engine::thisEngine;

Engine::Engine()
{
	thisEngine = this;
}

Engine::~Engine()
{
	
}

void Engine::init()
{
	graphicsEngine.init();
}

void Engine::run()
{
	graphicsEngine.run();
}
