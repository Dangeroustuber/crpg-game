#include "pch.h"

#include "SwapChain.hpp"
#include "Allocator.hpp"

#include <D3D12MemAlloc.h>

using namespace Microsoft::WRL;

namespace dx12 {
	bool SwapChain::initialize(ID3D12Device* device, ID3D12CommandQueue* commandQueue, Allocator* dx12Allocator, HWND hWnd, uint32_t inWidth, uint32_t inHeight) {
		width = inWidth;
		height = inHeight;

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = 2;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		tryHResult(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(rtvHeap.GetAddressOf())));
		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		tryHResult(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap.GetAddressOf())));
		dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = backBufferFormat;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = 0;

		ComPtr<IDXGISwapChain1> swapChain1;
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
#ifdef _DEBUG
		tryHResult(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(factory.GetAddressOf())));
#else
		tryHResult(CreateDXGIFactory2(0, IID_PPV_ARGS(factory.GetAddressOf())));
#endif

		tryHResult(factory->CreateSwapChainForHwnd(commandQueue, hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1));

		tryHResult(swapChain1.As(&swapchain));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (uint32_t i = 0; i < 2; ++i) {	// make rtv for each back buffer
			tryHResult(swapchain->GetBuffer(i, IID_PPV_ARGS(renderTargets[i].GetAddressOf())));
			device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, rtvDescriptorSize);
		}

		D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			depthStencilFormat,
			width,
			height,
			1,
			1,
			1,
			0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);

		D3D12_CLEAR_VALUE depthClearValue = {};
		depthClearValue.Format = depthStencilFormat;
		depthClearValue.DepthStencil.Depth = 1.0f;
		depthClearValue.DepthStencil.Stencil = 0;

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		tryHResult(dx12Allocator->getAllocator()->CreateResource(
			&allocDesc,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			&depthAllocation,
			IID_PPV_ARGS(depthStencil.GetAddressOf())));


		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthStencilFormat;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		device->CreateDepthStencilView(depthStencil.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());

		return true;
	}

	void SwapChain::present(uint32_t vsync, uint32_t presentFlags) {
		tryHResult(swapchain->Present(vsync, presentFlags));
	}

	uint32_t SwapChain::getBackBufferIndex() {
		return swapchain->GetCurrentBackBufferIndex();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::getDepthBufferCPUHandle() {
		return dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::getRenderTargetViewCPUHandle() {
		return rtvHeap->GetCPUDescriptorHandleForHeapStart();
	}
}