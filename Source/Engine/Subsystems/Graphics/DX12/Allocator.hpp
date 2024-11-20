#pragma once
#include "pch.h"

#include <D3D12MemAlloc.h>

// i suspect this can get much more complicated.
namespace dx12 {
	struct Allocator {
		bool initialize(ID3D12Device* device, IDXGIAdapter1* adapter);
		D3D12MA::Allocator* getAllocator();

		std::vector<D3D12MA::Allocation*> allocations;
		D3D12MA::Allocator* allocator;

		D3D12MA::Allocation* createResource(const D3D12MA::ALLOCATION_DESC& allocDesc, const D3D12_RESOURCE_DESC& resourceDesc, D3D12_RESOURCE_STATES state);

	};
}