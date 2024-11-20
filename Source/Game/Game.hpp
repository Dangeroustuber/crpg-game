#pragma once
#include "pch.h"
#include "IGame.hpp"
#include "Engine/Subsystems/Graphics/DX12/Shader.hpp"
#include "Engine/Subsystems/Graphics/DX12/UploadHeap.hpp"
#include "Engine/Subsystems/Graphics/DX12/DescriptorHeap.hpp"
#include "Engine/Camera/Camera.hpp"
#include "Engine/Subsystems/Graphics/DX12/ResourceManager.hpp"

#include <SimpleMath.h>
#include <D3D12MemAlloc.h>

struct CubeVertex {
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector4 color;
};

struct PerFrameData {
	DirectX::SimpleMath::Matrix viewProjection;
};

struct Game : IGame {
	bool initialize();
	void event(Event e);
	void update();
	void render();
	void shutdown();

	dx12::MeshHandle singleCube;

	Camera camera;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraBuffer;
	D3D12MA::Allocation* cameraAllocation = nullptr;
	uint32_t cameraBufferIndex;

	std::array<CubeVertex, 36> vertices;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12MA::Allocation* vertexBufferAllocation;
	uint32_t vertexBufferIndex;

	dx12::UploadHeap uploadHeap;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

	dx12::Shader vertexShader{ dx12::Vertex };
	dx12::Shader pixelShader{ dx12::Pixel };
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig;
	dx12::DescriptorHeap descriptorHeap;

	// temporary location
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
};
