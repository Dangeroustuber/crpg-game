#include "pch.h"

#include "Engine/Subsystems/Window.hpp"
#include "Engine/Subsystems/Graphics/DX12/ResourceManager.hpp"

#include "GraphicsContext.hpp"

namespace dx12 {
	bool GraphicsContext::initialize(Window* window) {
		if (!device.initialize()) {
			LOG_FATAL("Unable to initialize DirectX12 graphics device");
			return false;
		}

		if (!allocator.initialize(device.device.Get(), device.adapter.Get())) {
			LOG_FATAL("Unable to initialize allocator used for the game");
			return false;
		}

		if (!commandQueue.initialize(device.device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT)) {
			LOG_FATAL("Unable to initialize DirectX12 command queue");
			return false;
		}

		if (!swapchain.initialize(device.device.Get(), commandQueue.queue.Get(), &allocator, window->windowHandle, window->desc.width, window->desc.height)) {
			LOG_FATAL("Unable to initialize DirectX12 swapchain");
			return false;
		}

		return true;
	}

	void GraphicsContext::setupView() {
		D3D12_VIEWPORT viewport = { 0.0f, 0.0f,	(float)swapchain.width, (float)swapchain.height, 0.0f, 1.0f };
		D3D12_RECT scissorRect = { 0, 0, (int32_t)swapchain.width, (int32_t)swapchain.height };
		commandQueue.commandList->RSSetViewports(1, &viewport);
		commandQueue.commandList->RSSetScissorRects(1, &scissorRect);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GraphicsContext::getDSVHandle() {
		return swapchain.getDepthBufferCPUHandle();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GraphicsContext::getRTVHandle(uint32_t currentBackBufferIndex) {
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(swapchain.getRenderTargetViewCPUHandle(), currentBackBufferIndex, swapchain.rtvDescriptorSize);
	}
}