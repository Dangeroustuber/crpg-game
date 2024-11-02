#pragma once

#include <cstdint>

#include "Subsystems/Window.hpp"
#include "Utils/IniFileParser.hpp"

struct Engine {
	Engine();
	~Engine();

	void initialize();
	int32_t run();
	void shutdown();

	LRESULT CALLBACK handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	Window window;


	INIFileParser configFileParser;

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;
};