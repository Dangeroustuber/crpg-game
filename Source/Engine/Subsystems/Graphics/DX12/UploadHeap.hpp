#pragma once

#include "pch.h"

namespace dx12 {
	struct UploadHeap {
		void initialize(ID3D12Device* device, uint64_t size);
		bool uploadData(ID3D12GraphicsCommandList* commandList, ID3D12Resource* finalResource, std::span<std::byte> data);

		void release();

		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		uint8_t* mappedMemory = nullptr;
		size_t currentOffset = 0;
		size_t totalSize = 0;
	};
}