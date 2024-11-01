#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>

struct WindowDescription {
	LPCWSTR windowName;

	uint32_t width = 0;
	uint32_t height = 0;
};

struct Window {
	void initialize(const WindowDescription& inDesc, void* engine);

	WindowDescription desc;
	HWND windowHandle{ 0 };
	WNDCLASSEX windowClass{ 0 };
};