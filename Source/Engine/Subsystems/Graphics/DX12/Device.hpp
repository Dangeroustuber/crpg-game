#pragma once

#include "pch.h"

#include <dxgi.h>

#include "Utils/Utils.hpp"
namespace dx12 {
	struct Device {
		Microsoft::WRL::ComPtr<ID3D12Device> device;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

		ID3D12Device* getD3D12Device();

		bool initialize();
	};
}