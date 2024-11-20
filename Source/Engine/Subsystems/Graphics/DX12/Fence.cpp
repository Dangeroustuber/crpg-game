#include "pch.h"

#include "Fence.hpp"
#include "Utils/Utils.hpp"

namespace dx12 {
	bool Fence::initialize(ID3D12Device* device) {
		handle = CreateEvent(nullptr, false, false, nullptr);

		if (!handle) {
			LOG_ERROR("Unable to initialize fence");
			return false;
		}

		tryHResult(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));

		return true;
	}

	void Fence::signal(ID3D12CommandQueue* queue, uint64_t inValue) {
		tryHResult(queue->Signal(fence.Get(), inValue));
	}

	bool Fence::waitOnValue(uint64_t inValue, DWORD timeoutMs) {
		if (fence->GetCompletedValue() >= inValue) {
			return true;
		}

		fence->SetEventOnCompletion(inValue, handle);
		return WaitForSingleObject(handle, timeoutMs) == WAIT_OBJECT_0;
	}
}