#pragma once
#include "pch.h"

namespace dx12 {
	struct DescriptorHeap {
		void initialize(ID3D12Device* inDevice, uint32_t numDescriptors);

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
		uint32_t descriptorSize;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuStart;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuStart;

		uint32_t currentResourceIndex = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index);

		uint32_t CreateBufferSRV(ID3D12Device* device, ID3D12Resource* resource, uint32_t elementSize, uint32_t numElements);

		uint32_t CreateBufferCBV(ID3D12Device* device, ID3D12Resource* resource, uint32_t size);
	};
}