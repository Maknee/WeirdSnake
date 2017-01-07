#include "Engine.h"

Engine::Engine()
{

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
