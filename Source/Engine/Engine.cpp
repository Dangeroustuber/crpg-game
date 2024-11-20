#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include "Engine/Engine.hpp"
#include "Game/IGame.hpp"
#include "Subsystems/Graphics/DX12/GraphicsContext.hpp"
#include "Subsystems/Graphics/DX12/ResourceManager.hpp"

dx12::GraphicsContext* Engine::graphicsContext = nullptr;
dx12::ResourceManager* Engine::resourceManager = nullptr;

Engine::Engine() {}
Engine::~Engine() {}

void Engine::initialize() {
	// initialize logging subsystem first
	Logger::initialize("crpg-game");
	LOG_INFO("Initialized logging subsystem");

	// read in the engine/game configuration
	auto configurationFileData = configFileParser.parse("config.ini");

	if (configurationFileData.empty()) {
		LOG_FATAL("Unable to read engine configuration file successfully.\nexiting...");
		std::exit(EXIT_FAILURE);
	}

	LOG_INFO("Successfully read the engine configuration file"); // config file does not do anything yet.

	// initialize all the other engine subsystems
	window.initialize({ .windowName = L"tempname", .width = 800, .height = 600 }, this);
	LOG_INFO("Initialized window subsystem");

	graphicsContext = new dx12::GraphicsContext();

	if (!graphicsContext->initialize(&window)) {
		LOG_FATAL("Unable to initialize the graphics subsystem.\nexiting...");
		std::exit(EXIT_FAILURE);
	}

	resourceManager = new dx12::ResourceManager();
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

void Engine::shutdown() {
	// write shutdown code eventually. Should first wait for gpu
}

LRESULT Engine::handleMessage(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_QUIT: [[fallthrough]];
		case WM_CLOSE: [[fallthrough]];
		case WM_DESTROY:
		{
			shutdownEngine = true;
			PostQuitMessage(0);
			break;
		}
		case WM_LBUTTONDOWN:
		{		
			pumpEvent({.type = Event::Type::MouseButtonDown,
					  .mouseButton = Event::MouseButton::Left, 
					  .mousePressX = GET_X_LPARAM(lParam), 
					  .mousePressY = GET_Y_LPARAM(lParam)});
			break;
		}
		case WM_RBUTTONDOWN:
		{
			pumpEvent({.type = Event::Type::MouseButtonDown,
					  .mouseButton = Event::MouseButton::Right,
					  .mousePressX = GET_X_LPARAM(lParam),
					  .mousePressY = GET_Y_LPARAM(lParam)});
			break;
		}
		case WM_MOUSEMOVE:
		{
			pumpEvent({.type = Event::Type::MouseMove, .mouseMoveX = GET_X_LPARAM(lParam), .mouseMoveY = GET_Y_LPARAM(lParam)});
			break;
		}
		case WM_KEYDOWN:
		{ 
		    pumpEvent({.type = Event::Type::KeyDown, .keyCode = mapWindowsKeyCode(wParam)});
			break;
		}
		case WM_KEYUP:
		{
			pumpEvent({.type = Event::Type::KeyUp, .keyCode = mapWindowsKeyCode(wParam)});
			break;
		}
		default:
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

inline void Engine::pumpEvent(Event e) {
	assert(game);

	game->event(e);
}

Event::Key Engine::mapWindowsKeyCode(WPARAM key) {
	switch (key) {
		case 'W': return Event::Key::W;
		case 'A': return Event::Key::A;
		case 'S': return Event::Key::S;
		case 'D': return Event::Key::D;
		default: return Event::Key::Unknown;
	}
}
