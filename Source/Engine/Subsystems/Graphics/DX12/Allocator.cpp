#include "pch.h"
#include "Allocator.hpp"
#include "Utils/Utils.hpp"

namespace dx12 {
	bool Allocator::initialize(ID3D12Device* device, IDXGIAdapter1* adapter) {
		D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
		allocatorDesc.pDevice = device;
		allocatorDesc.pAdapter = adapter;

		return SUCCEEDED(D3D12MA::CreateAllocator(&allocatorDesc, &allocator));
	}

	D3D12MA::Allocator* Allocator::getAllocator() {
		return allocator;
	}

	D3D12MA::Allocation* Allocator::createResource(const D3D12MA::ALLOCATION_DESC& allocDesc, const D3D12_RESOURCE_DESC& resourceDesc, D3D12_RESOURCE_STATES state) {
		D3D12MA::Allocation* allocation = nullptr;
		ID3D12Resource* resource = nullptr;

		tryHResult(allocator->CreateResource(&allocDesc, &resourceDesc, state, nullptr, &allocation, IID_PPV_ARGS(&resource)));

		if (allocation) {
			resource->Release();
			allocations.push_back(allocation);

			return allocation;
		} else {
			LOG_ERROR("Unable to create dx12 resource");
			return nullptr;
		}
	}
}
