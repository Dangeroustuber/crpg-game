#include "pch.h"
#include "DescriptorHeap.hpp"

namespace dx12 {
	void DescriptorHeap::initialize(ID3D12Device* inDevice, uint32_t numDescriptors) {
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = numDescriptors;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		inDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
		descriptorSize = inDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		cpuStart = heap->GetCPUDescriptorHandleForHeapStart();
		gpuStart = heap->GetGPUDescriptorHandleForHeapStart();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(uint32_t index) {
		D3D12_CPU_DESCRIPTOR_HANDLE handle = cpuStart;
		handle.ptr += index * descriptorSize;
		return handle;
	}

	uint32_t DescriptorHeap::CreateBufferSRV(ID3D12Device* device, ID3D12Resource* resource, uint32_t elementSize, uint32_t numElements) {
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = numElements;
		srvDesc.Buffer.StructureByteStride = elementSize;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		device->CreateShaderResourceView(resource, &srvDesc, GetCPUHandle(currentResourceIndex));
		return currentResourceIndex++;
	}

	uint32_t DescriptorHeap::CreateBufferCBV(ID3D12Device* device, ID3D12Resource* resource, uint32_t size) {
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = alignTo256Bytes(size);

		device->CreateConstantBufferView(&cbvDesc, GetCPUHandle(currentResourceIndex));
		return currentResourceIndex++;
	}
}