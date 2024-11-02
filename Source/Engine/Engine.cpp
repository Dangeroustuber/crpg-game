#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Engine/Engine.hpp"
#include "Subsystems/Logger.hpp"

Engine::Engine() {}
Engine::~Engine() {}

void Engine::initialize() {
	// initialize logging subsystem first
	Logger::initialize("crpg-game");
	LOG_INFO("Initialized logging subsystem");

	// read in the engine/game configuration
	configFileParser.parse("config.ini");

	// initialize all the other engine subsystems
	window.initialize({ .windowName = L"tempname", .width = 800, .height = 600 }, this);
	LOG_INFO("Initialized window subsystem");
}

int32_t Engine::run() {
	initialize();

	// game loop

	shutdown();

	return 0;
}

void Engine::shutdown() {}

LRESULT Engine::handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	return 0;
}
