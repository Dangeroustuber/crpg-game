#pragma once

struct IGame {
	IGame() = default;
	virtual ~IGame() {}

	virtual bool initialize() = 0;

	virtual void update() = 0;
	virtual void render() = 0;

	virtual void shutdown() = 0;
};