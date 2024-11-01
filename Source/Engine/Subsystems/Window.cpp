#include <cassert>

#include "Window.hpp"
#include "Engine/Engine.hpp"
#include "Logger.hpp"


static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	Engine* engine = reinterpret_cast<Engine*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	// when the window is initially created we set passed in user parameter from when we created the window
	if (msg == WM_CREATE) {
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		return 0;
	}

	// we forward all the messages to the engine. We do not handle all of them necessarily.
	if (engine) {
		return engine->handleMessage(hwnd, msg, wParam, lParam);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


void Window::initialize(const WindowDescription& inDesc, void* engine) {
	desc = inDesc;

	windowClass = {
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = wndProc,
		.hInstance = GetModuleHandle(nullptr),
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		.lpszClassName = L"windowClass"
	};

	if (!RegisterClassEx(&windowClass)) {
		LOG_FATAL("Unable to register window class in the initialize function for the window");
		std::exit(-1);
	}

	assert(&windowClass);

	RECT rect{ 0, 0, (LONG)desc.width, (LONG)desc.height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	int32_t monitorWidth = GetSystemMetrics(SM_CXSCREEN);
	int32_t monitorHeight = GetSystemMetrics(SM_CYSCREEN);

	int32_t middleX = (monitorWidth - desc.width) / 2;
	int32_t middleY = (monitorHeight - desc.height) / 2;

	windowHandle = CreateWindowEx(
		WS_EX_NOREDIRECTIONBITMAP,
		windowClass.lpszClassName,
		desc.windowName,
		WS_OVERLAPPEDWINDOW,
		middleX,
		middleY,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		GetModuleHandle(nullptr), // get the default hinstance of the process
		engine // pass the engine so that we can hook into into message handling for the window.
	);

	if (windowHandle == nullptr) {
		LOG_FATAL("Unable to create window");
		std::exit(-1);
	}

	ShowWindow(windowHandle, SW_SHOW);
	UpdateWindow(windowHandle);
}
