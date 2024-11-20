#include "Engine/Engine.hpp"
#include "Game/Game.hpp"

int main() {
	Game game;
	Engine engine;
	return engine.run(&game);
}