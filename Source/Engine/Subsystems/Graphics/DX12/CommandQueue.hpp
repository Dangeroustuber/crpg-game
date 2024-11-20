#pragma once
#include "pch.h"

#include "Fence.hpp"

namespace dx12 {
	struct CommandQueue {
		bool initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);

		void begin();
		void end();
		void waitForFrame(uint32_t frameIndex);
		void waitForGPU();

		ID3D12GraphicsCommandList* getCommandList();

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[2];
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

		Fence fence;
		uint64_t fenceValues[2] = {};
		uint32_t currentFrame = 0;
	};
}