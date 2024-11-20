#include "pch.h"
#include "Game.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Subsystems/Graphics/DX12/GraphicsContext.hpp"

using namespace dx12;
using namespace Microsoft::WRL;

bool Game::initialize() {
	auto graphicsContext = Engine::graphicsContext;
	auto device = graphicsContext->device.getD3D12Device();

	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler.GetAddressOf()));
	DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils.GetAddressOf()));

	{
		auto makeVertex = [](float x, float y, float z, const DirectX::SimpleMath::Vector4& color) {
			return CubeVertex{ DirectX::SimpleMath::Vector3(x, y, z), color };
		};

		auto red = DirectX::SimpleMath::Vector4{ 1.0f, 0.0f, 0.0f, 1.0f };
		auto green = DirectX::SimpleMath::Vector4{ 0.0f, 1.0f, 0.0f, 1.0f };
		auto blue = DirectX::SimpleMath::Vector4{ 0.0f, 0.0f, 1.0f, 1.0f };
		auto yellow = DirectX::SimpleMath::Vector4{ 1.0f, 1.0f, 0.0f, 1.0f };
		auto purple = DirectX::SimpleMath::Vector4{ 1.0f, 0.0f, 1.0f, 1.0f };
		auto cyan = DirectX::SimpleMath::Vector4{ 0.0f, 1.0f, 1.0f, 1.0f };

		vertices = {
			makeVertex(-0.5f, -0.5f, -0.5f, red), makeVertex(-0.5f,  0.5f, -0.5f, red), makeVertex(0.5f,  0.5f, -0.5f, red),
			makeVertex(-0.5f, -0.5f, -0.5f, red), makeVertex(0.5f,  0.5f, -0.5f, red), makeVertex(0.5f, -0.5f, -0.5f, red),
			makeVertex(-0.5f, -0.5f,  0.5f, green), makeVertex(0.5f,  0.5f,  0.5f, green), makeVertex(-0.5f,  0.5f,  0.5f, green),
			makeVertex(-0.5f, -0.5f,  0.5f, green), makeVertex(0.5f, -0.5f,  0.5f, green), makeVertex(0.5f,  0.5f,  0.5f, green),
			makeVertex(-0.5f,  0.5f, -0.5f, blue), makeVertex(-0.5f,  0.5f,  0.5f, blue), makeVertex(0.5f,  0.5f,  0.5f, blue),
			makeVertex(-0.5f,  0.5f, -0.5f, blue), makeVertex(0.5f,  0.5f,  0.5f, blue), makeVertex(0.5f,  0.5f, -0.5f, blue),
			makeVertex(-0.5f, -0.5f, -0.5f, yellow), makeVertex(0.5f, -0.5f,  0.5f, yellow), makeVertex(-0.5f, -0.5f,  0.5f, yellow),
			makeVertex(-0.5f, -0.5f, -0.5f, yellow), makeVertex(0.5f, -0.5f, -0.5f, yellow), makeVertex(0.5f, -0.5f,  0.5f, yellow),
			makeVertex(0.5f, -0.5f, -0.5f, purple), makeVertex(0.5f,  0.5f,  0.5f, purple), makeVertex(0.5f, -0.5f,  0.5f, purple),
			makeVertex(0.5f, -0.5f, -0.5f, purple), makeVertex(0.5f,  0.5f, -0.5f, purple), makeVertex(0.5f,  0.5f,  0.5f, purple),
			makeVertex(-0.5f, -0.5f, -0.5f, cyan), makeVertex(-0.5f, -0.5f,  0.5f, cyan), makeVertex(-0.5f,  0.5f,  0.5f, cyan),
			makeVertex(-0.5f, -0.5f, -0.5f, cyan), makeVertex(-0.5f,  0.5f,  0.5f, cyan), makeVertex(-0.5f,  0.5f, -0.5f, cyan)
		};
	}


	descriptorHeap.initialize(device, 10000);
	uploadHeap.initialize(device, 32 * 1024 * 1024);

	singleCube = Engine::resourceManager->loadMesh("Source/Assets/Meshes/cube.glb", graphicsContext, &uploadHeap, &descriptorHeap);

	// Create root signature with bindless flag
	CD3DX12_ROOT_PARAMETER rootParams[1];
	rootParams[0].InitAsConstants(2, 0); // Just pass descriptor indices as root constants

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.Init(_countof(rootParams), rootParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	tryHResult(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &error));
	device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSig.GetAddressOf()));

	{
		// Create vertex buffer
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		auto vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size() * sizeof(CubeVertex));

		if (FAILED(graphicsContext->allocator.getAllocator()->CreateResource(
			&allocDesc,
			&vertexBufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			&vertexBufferAllocation,
			IID_PPV_ARGS(&vertexBuffer)))) {
			LOG_ERROR("Failed to create vertex buffer");
			return false;
		}

		graphicsContext->commandQueue.begin();
		auto cmdList = graphicsContext->commandQueue.commandList.Get();

		auto vertexData = std::span<std::byte>{ reinterpret_cast<std::byte*>(vertices.data()), vertices.size() * sizeof(CubeVertex) };

		uploadHeap.uploadData(cmdList, vertexBuffer.Get(), vertexData);

		graphicsContext->commandQueue.end();
		graphicsContext->commandQueue.waitForGPU();

		vertexBufferIndex = descriptorHeap.CreateBufferSRV(device, vertexBuffer.Get(), sizeof(CubeVertex), (uint32_t)vertices.size());
	}

	{
		// set camera
		camera.initialize((float)graphicsContext->swapchain.width, (float)graphicsContext->swapchain.height, 0.1f, 100.0f);

		// camera constant buffer, matrices
		{
			D3D12MA::ALLOCATION_DESC allocDesc = {};
			allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

			auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(PerFrameData) + 255) & ~255);

			if (FAILED(graphicsContext->allocator.getAllocator()->CreateResource(
				&allocDesc,
				&bufferDesc,
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				&cameraAllocation,
				IID_PPV_ARGS(&cameraBuffer)))) {
				LOG_ERROR("Failed to create camera buffer");
				return false;
			}

			cameraBufferIndex = descriptorHeap.CreateBufferCBV(device, cameraBuffer.Get(), sizeof(PerFrameData));
		}
	}

	std::vector<LPCWSTR> vertexShaderArgs{
		L"vertex.hlsl",
		L"/Zpr", // so we dont have to call .transform() on the matrices
		L"/E",
		L"VSMain",
		L"/T",
		L"vs_6_6",
		L"/Zi",
		L"/HV",
		L"2021"
	};

	std::vector<LPCWSTR> pixelShaderArgs{
		L"pixel.hlsl",
		L"/Zpr", // so we dont have to call .transform() on the matrices
		L"/E",
		L"PSMain",
		L"/T",
		L"ps_6_6",
		L"/Zi",
		L"/HV",
		L"2021"
	};

	if (!vertexShader.compile(L"Source/Assets/Shaders/vertex.hlsl", dxcCompiler.Get(), dxcUtils.Get(), vertexShaderArgs)) {
		LOG_ERROR("Failed to compile vertex shader");
		return false;
	}

	if (!pixelShader.compile(L"Source/Assets/Shaders/pixel.hlsl", dxcCompiler.Get(), dxcUtils.Get(), pixelShaderArgs)) {
		LOG_ERROR("Failed to compile pixel shader");
		return false;
	}

	// simple pso, read about pso caching
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { nullptr, 0 };
	psoDesc.pRootSignature = rootSig.Get();
	psoDesc.VS = { .pShaderBytecode = vertexShader.blob.Get()->GetBufferPointer(), .BytecodeLength = vertexShader.blob.Get()->GetBufferSize() };
	psoDesc.PS = { .pShaderBytecode = pixelShader.blob.Get()->GetBufferPointer(), .BytecodeLength = pixelShader.blob.Get()->GetBufferSize() };
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;

	tryHResult(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));

	return true;
}

void Game::event(Event e) {
	(e);
}

void Game::update() {
	auto graphicsContext = Engine::graphicsContext;
	auto cmdList = graphicsContext->commandQueue.commandList.Get();

	{
		graphicsContext->commandQueue.begin();
		PerFrameData cameraData;
		cameraData.viewProjection = camera.getViewProjectionMatrix();
		uploadHeap.uploadData(cmdList, cameraBuffer.Get(), std::span<std::byte>{reinterpret_cast<std::byte*>(&cameraData), sizeof(PerFrameData)});
		graphicsContext->commandQueue.end();
	}
}

void Game::render() {
	auto graphicsContext = Engine::graphicsContext;
	auto cmdList = graphicsContext->commandQueue.commandList.Get();
	uint32_t currentBackBufferIndex = graphicsContext->swapchain.getBackBufferIndex();

	graphicsContext->commandQueue.begin();

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(graphicsContext->swapchain.renderTargets[currentBackBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmdList->ResourceBarrier(1, &barrier);

	{
		auto rtvHandle = graphicsContext->getRTVHandle(currentBackBufferIndex);
		auto dsvHandle = graphicsContext->getDSVHandle();

		float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	}

	{
		auto cubeMesh = Engine::resourceManager->getMesh(singleCube);

		cmdList->SetPipelineState(pipelineState.Get());
		cmdList->SetGraphicsRootSignature(rootSig.Get());

		ID3D12DescriptorHeap* heaps[] = { descriptorHeap.heap.Get() };
		cmdList->SetDescriptorHeaps(_countof(heaps), heaps);
		cmdList->SetGraphicsRoot32BitConstant(0, cubeMesh->vertexBufferIndex, 0);
		cmdList->SetGraphicsRoot32BitConstant(0, cameraBufferIndex, 1);

		graphicsContext->setupView();
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (cubeMesh->hasIndices) {
			D3D12_INDEX_BUFFER_VIEW ibView{};
			ibView.BufferLocation = cubeMesh->indexBuffer->GetGPUVirtualAddress();
			ibView.Format = DXGI_FORMAT_R32_UINT;
			ibView.SizeInBytes = (uint32_t)cubeMesh->indices.size() * sizeof(uint32_t);

			cmdList->IASetIndexBuffer(&ibView);
			cmdList->DrawIndexedInstanced((uint32_t)cubeMesh->indices.size(), 1, 0, 0, 0);
		} else {
			cmdList->DrawInstanced((uint32_t)cubeMesh->vertices.size(), 1, 0, 0);
		}
	}

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(graphicsContext->swapchain.renderTargets[currentBackBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	cmdList->ResourceBarrier(1, &barrier);

	graphicsContext->commandQueue.end();
	graphicsContext->swapchain.present(1, 0);
}

void Game::shutdown() {

}
