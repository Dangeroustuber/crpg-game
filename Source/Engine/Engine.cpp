#include "Engine/Engine.hpp"

void Engine::initialize() {}

int32_t Engine::run() {
	initialize();

	// game loop

	shutdown();

	return 0;
}

void Engine::shutdown() {}
