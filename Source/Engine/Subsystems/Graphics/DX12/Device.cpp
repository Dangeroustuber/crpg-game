#include "pch.h"

#include "Device.hpp"
#include "Utils/Utils.hpp"

#include <dxgi.h>
#include <dxgi1_4.h>
#include <ranges>

using namespace Microsoft::WRL;

struct DX12Adapter {
	uint32_t selectedAdapterIndex;
	DXGI_ADAPTER_DESC1 desc;
	std::string name;
	D3D_FEATURE_LEVEL maxFeatureLevel;
};

static const char* featureLevelToString(D3D_FEATURE_LEVEL level) {
	switch (level) {
		case D3D_FEATURE_LEVEL_11_0: return "11_0";
		case D3D_FEATURE_LEVEL_11_1: return "11_1";
		case D3D_FEATURE_LEVEL_12_0: return "12_0";
		case D3D_FEATURE_LEVEL_12_1: return "12_1";
		case D3D_FEATURE_LEVEL_12_2: return "12_2";
		default: return "Unknown feature level";
	}
};


static std::vector<DX12Adapter> getInfoAboutAdaptersOnSystem() {
	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
#ifdef _DEBUG
	tryHResult(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(factory.GetAddressOf())));
#else
	tryHResult(CreateDXGIFactory2(0, IID_PPV_ARGS(factory.GetAddressOf())));
#endif

	std::vector<DX12Adapter> adapters;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> currentAdapter;

	std::array<D3D_FEATURE_LEVEL, 5> featureLevels{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	for (uint32_t i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, currentAdapter.GetAddressOf()); ++i) {
		DX12Adapter info;
		info.selectedAdapterIndex = i;
		currentAdapter->GetDesc1(&info.desc);

		{
			std::wstring wstr{ info.desc.Description };

			int32_t size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int32_t)wstr.size(), nullptr, 0, nullptr, nullptr);
			info.name.resize(size);

			WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int32_t)wstr.size(), &info.name.front(), size, nullptr, nullptr);
		}

		info.maxFeatureLevel = D3D_FEATURE_LEVEL_11_0; // we start backwards and check.
		for (auto level : featureLevels) {
			if (SUCCEEDED(D3D12CreateDevice(currentAdapter.Get(), level, _uuidof(ID3D12Device), nullptr))) {
				info.maxFeatureLevel = level;
				break;
			}
		}

		LOG_INFO("Adapter number {} :", i);
		LOG_INFO("\tName: {}", info.name);
		LOG_INFO("\tDedicated Video Memory: {} MB", info.desc.DedicatedVideoMemory / 1024 / 1024);
		LOG_INFO("\tDedicated System Memory: {} MB", info.desc.DedicatedSystemMemory / 1024 / 1024);
		LOG_INFO("\tShared System Memory: {} MB", info.desc.SharedSystemMemory / 1024 / 1024);
		LOG_INFO("\tFeature Level: {}", featureLevelToString(info.maxFeatureLevel));
		LOG_INFO("\tIs WARP: {}", (info.desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) ? "Yes" : "No");

		if (info.desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			LOG_WARN("Skipped adding software adapter to adapter list");
			continue;
		}

		adapters.push_back(std::move(info));
	}

	return adapters;
}

namespace dx12 {
	ID3D12Device* Device::getD3D12Device() {
		return device.Get();
	}

	bool Device::initialize() {
		auto allAdapters = getInfoAboutAdaptersOnSystem();

		if (allAdapters.empty()) {
			LOG_FATAL("No DirectX12 compatible adapters found");
			return false;
		}

		auto bestAdapter = std::ranges::max_element(allAdapters, [](const DX12Adapter& a, const DX12Adapter& b) {
			if (a.maxFeatureLevel != b.maxFeatureLevel) {
				return a.maxFeatureLevel < b.maxFeatureLevel;
			}

			return a.desc.DedicatedVideoMemory < b.desc.DedicatedVideoMemory;
		});

		if (bestAdapter == allAdapters.end()) {
			LOG_ERROR("Failed to select adapter from list of adapters.");
			return false;
		}

		LOG_INFO("\nSelected best adapter:");
		LOG_INFO("\tName: {}", bestAdapter->name);
		LOG_INFO("\tFeature Level: {}", featureLevelToString(bestAdapter->maxFeatureLevel));
		LOG_INFO("\tDedicated Video Memory: {} MB", bestAdapter->desc.DedicatedVideoMemory / 1024 / 1024);

		Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
#ifdef _DEBUG
		tryHResult(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(factory.GetAddressOf())));
#else
		tryHResult(CreateDXGIFactory2(0, IID_PPV_ARGS(factory.GetAddressOf())));
#endif


		factory->EnumAdapters1(bestAdapter->selectedAdapterIndex, adapter.GetAddressOf());

		{
#ifdef _DEBUG
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
				debugController->EnableDebugLayer();
				LOG_INFO("DX12 debug layer enabled");
			} else {
				LOG_WARN("Failed to enable DX12 debug layer. Debug interface is not available.");
			}
#endif

			tryHResult(D3D12CreateDevice(adapter.Get(), (*bestAdapter).maxFeatureLevel, IID_PPV_ARGS(device.GetAddressOf())));

#ifdef _DEBUG
			ComPtr<ID3D12InfoQueue> infoQueue;
			if (SUCCEEDED(device.As(&infoQueue))) {
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

				LOG_INFO("Enabled DX12 debug info queue");
			} else {
				LOG_INFO("Unable to setup debug info queue");
			}
#endif
		}

		CD3DX12FeatureSupport features;
		tryHResult(features.Init(device.Get()));

		if (features.HighestShaderModel() < D3D_SHADER_MODEL_6_6) {
			LOG_FATAL("We require at least shader model version 6.6 for the engine graphics subsystem");
			return false;
		}

		if (features.HighestRootSignatureVersion() < D3D_ROOT_SIGNATURE_VERSION_1_1) {
			LOG_FATAL("We require at least root signature version 1_1 for the engine graphics subsystem");
			return false;
		}

		return true;
	}
}