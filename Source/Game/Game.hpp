#pragma once

#include "IGame.hpp"

struct Game : IGame {
	bool initialize();
	void update();
	void render();
	void shutdown();
};
