#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Engine/Engine.hpp"
#include "Subsystems/Logger.hpp"
#include "Game/IGame.hpp"

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

int32_t Engine::run(IGame* inGame) {
	game = inGame;

	initialize();
	LOG_INFO("Finished initializing engine subsystems");

	if (!game->initialize()) { // The game should log it's problems if it fails to initialize.
		LOG_FATAL("Errors occured when attempting to initialize the game");
		return EXIT_FAILURE;
	}

	while (!shutdownEngine) {
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				break;
			}
		}

		game->update();
		game->render();
	}

	game->shutdown();
	shutdown();

	return 0;
}

void Engine::shutdown() {}

LRESULT Engine::handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_QUIT: [[fallthrough]];
		case WM_CLOSE: [[fallthrough]];
		case WM_DESTROY:
		{
			shutdownEngine = true;
			PostQuitMessage(0);
			break;
		}
		default:
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}