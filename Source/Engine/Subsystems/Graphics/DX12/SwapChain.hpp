#pragma once
#include "pch.h"

#include <dxgi1_6.h>
#include <D3D12MemAlloc.h>

#include "Device.hpp"
#include "Allocator.hpp"

namespace dx12 {
	struct SwapChain {
		bool initialize(ID3D12Device* device, ID3D12CommandQueue* commandQueue, Allocator* dx12Allocator, HWND hWnd, uint32_t width, uint32_t height);
		void present(uint32_t vsync, uint32_t presentFlags);

		uint32_t getBackBufferIndex();
		D3D12_CPU_DESCRIPTOR_HANDLE getDepthBufferCPUHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE getRenderTargetViewCPUHandle();

		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain;

		D3D12MA::Allocation* depthAllocation; // i believe its ok to hold this here.

		Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[2];
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t rtvDescriptorSize = 0;
		uint32_t dsvDescriptorSize = 0;
		uint32_t currentBackBufferIndex = 0;

		DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D32_FLOAT;
	};
}