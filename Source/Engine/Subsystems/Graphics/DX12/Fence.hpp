#pragma once
#include "pch.h"

#include "Device.hpp"
namespace dx12 {
	struct Fence {
		bool initialize(ID3D12Device* device);

		void signal(ID3D12CommandQueue* queue, uint64_t value);
		bool waitOnValue(uint64_t value, DWORD timeoutMs = INFINITE);

		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		HANDLE handle;
		uint64_t value;
	};
}