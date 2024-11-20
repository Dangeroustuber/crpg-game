#pragma once
#include "pch.h"

#include "Subsystems/Window.hpp"
#include "Utils/IniFileParser.hpp"
#include "Subsystems/Events/Event.hpp"

struct IGame;
namespace dx12 {
	struct GraphicsContext;
	struct ResourceManager;
}

struct Engine {
	Engine();
	~Engine();

	void initialize();
	int32_t run(IGame* inGame);
	void shutdown();

	LRESULT CALLBACK handleMessage(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam);

	Window window;
	INIFileParser configFileParser;
	static dx12::GraphicsContext* graphicsContext;
	static dx12::ResourceManager* resourceManager;

	// Engine specific
private:
	inline void pumpEvent(Event e);
	bool shutdownEngine = false; 
	IGame* game;
	Event::Key mapWindowsKeyCode(WPARAM key);
};