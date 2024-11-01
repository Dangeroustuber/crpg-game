#pragma once

#include <cstdint>

struct Engine {
	void initialize();
	int32_t run();
	void shutdown();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;
};