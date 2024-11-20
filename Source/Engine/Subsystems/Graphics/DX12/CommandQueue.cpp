#include "pch.h"

#include "CommandQueue.hpp"
namespace dx12 {


	bool CommandQueue::initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type) {
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = type;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		tryHResult(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue)));

		for (uint32_t i = 0; i < 2; i++) {
			tryHResult(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocators[i])));
		}

		if (!fence.initialize(device)) {
			return false;
		} 

		tryHResult(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf())));

		// created in an open state.
		tryHResult(commandList->Close());

		return true;
	}

	void CommandQueue::begin() {
		waitForFrame(currentFrame);

		tryHResult(commandAllocators[currentFrame]->Reset());
		tryHResult(commandList->Reset(commandAllocators[currentFrame].Get(), nullptr));
	}

	void CommandQueue::end() {
		tryHResult(commandList->Close());

		ID3D12CommandList* const ppCommandLists[] = { commandList.Get() };
		queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		fenceValues[currentFrame] = ++fence.value;
		fence.signal(queue.Get(), fence.value);

		currentFrame = (currentFrame + 1) % 2;
	}

	void CommandQueue::waitForFrame(uint32_t frameIndex) {
		if (fenceValues[frameIndex] != 0 && fence.fence->GetCompletedValue() < fenceValues[frameIndex]) {
			fence.waitOnValue(fenceValues[frameIndex]);
		}
	}

	void CommandQueue::waitForGPU() {
		for (uint32_t i = 0; i < 2; i++) {
			waitForFrame(i);
		}
	}

	ID3D12GraphicsCommandList* CommandQueue::getCommandList() {
		return commandList.Get();
	}
}