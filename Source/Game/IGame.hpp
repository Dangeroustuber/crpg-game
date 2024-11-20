#pragma once
#include "pch.h"

#include "../Source/Engine/Subsystems/Events/Event.hpp"

struct IGame {
	IGame() = default;
	virtual ~IGame() {}

	virtual bool initialize() = 0;

	virtual void event(Event e) = 0;
	virtual void update() = 0;
	virtual void render() = 0;

	virtual void shutdown() = 0;
};