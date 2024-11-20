#pragma once

#include "pch.h"

#include "SwapChain.hpp"
#include "Fence.hpp"
#include "Device.hpp"
#include "CommandQueue.hpp"
#include "Allocator.hpp"

struct Window;

namespace dx12 {
	struct GraphicsContext {
		bool initialize(Window* window);

		void setupView();

		D3D12_CPU_DESCRIPTOR_HANDLE getDSVHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE getRTVHandle(uint32_t currentBackBufferIndex);

		Device device;

		Allocator allocator;

		CommandQueue commandQueue;

		Fence fence;

		SwapChain swapchain;
	};
}