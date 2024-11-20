#include "pch.h"
#include "UploadHeap.hpp"
namespace dx12 {

	void UploadHeap::initialize(ID3D12Device* device, uint64_t size) {
		totalSize = size;

		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);

		tryHResult(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer)));

		tryHResult(uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedMemory)));
	}

	bool UploadHeap::uploadData(ID3D12GraphicsCommandList* commandList, ID3D12Resource* finalResource, std::span<std::byte> data) {
		if ((currentOffset + data.size_bytes()) > totalSize) {
			LOG_ERROR("Upload heap out of space");
			return false;
		}

		// put data first in upload heap
		std::memcpy(mappedMemory + currentOffset, data.data(), data.size_bytes());

		// set the destination where the data is supposed to go in the copy destination state
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(finalResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		commandList->ResourceBarrier(1, &barrier);

		// then from the upload heap into the destination
		commandList->CopyBufferRegion(finalResource, 0, uploadBuffer.Get(), currentOffset, data.size_bytes());

		// Then back to the common state, which is likely the state the data began in.
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(finalResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
		commandList->ResourceBarrier(1, &barrier);

		currentOffset += data.size_bytes();
		currentOffset = (currentOffset + 255) & ~255;

		return true;
	}

	void UploadHeap::release() {
		if (uploadBuffer) {
			uploadBuffer->Unmap(0, nullptr);
			mappedMemory = nullptr;
		}

		currentOffset = 0;
	}
}