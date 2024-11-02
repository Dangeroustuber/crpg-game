#include "Engine/Engine.hpp"
#include "Game/Game.hpp"

int main(int argc, char* argv[]) {
	Game game;
	Engine engine;
	return engine.run(&game);
}