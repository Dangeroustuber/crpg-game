#include "Mesh.hpp"

namespace dx12 {

	bool Mesh::initialize(GraphicsContext* graphicsContext, UploadHeap* uploadHeap, DescriptorHeap* descriptorHeap) {
		auto device = graphicsContext->device.getD3D12Device();

		// create vertex buffer
		{
			D3D12MA::ALLOCATION_DESC allocDesc = {};
			allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

			auto vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size() * sizeof(MeshVertex));

			auto vertexAlloc = graphicsContext->allocator.createResource(allocDesc, vertexBufferDesc, D3D12_RESOURCE_STATE_COPY_DEST);

			if (!vertexAlloc) {
				return false;
			}

			vertexBuffer = vertexAlloc->GetResource();

			graphicsContext->commandQueue.begin();

			uploadHeap->uploadData(graphicsContext->commandQueue.commandList.Get(),
								   vertexBuffer,
								   std::span<std::byte>{ reinterpret_cast<std::byte*>(vertices.data()), vertices.size() * sizeof(MeshVertex)});

			graphicsContext->commandQueue.end();
			graphicsContext->commandQueue.waitForGPU();


			vertexBufferIndex = descriptorHeap->CreateBufferSRV(device, vertexBuffer, sizeof(MeshVertex), (uint32_t)vertices.size());
		}

		// create index buffer if index data is present
		if (hasIndices) {
			D3D12MA::ALLOCATION_DESC allocDesc = {};
			allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

			auto indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(uint32_t));

			auto indexAlloc = graphicsContext->allocator.createResource(allocDesc, indexBufferDesc, D3D12_RESOURCE_STATE_COPY_DEST);

			if (!indexAlloc) {
				return false;
			}

			indexBuffer = indexAlloc->GetResource();

			graphicsContext->commandQueue.begin();

			uploadHeap->uploadData(graphicsContext->commandQueue.commandList.Get(),
								   indexBuffer,
								   std::span<std::byte>{reinterpret_cast<std::byte*>(indices.data()), indices.size() * sizeof(uint32_t)});

			graphicsContext->commandQueue.end();
			graphicsContext->commandQueue.waitForGPU();

			indexBufferIndex = descriptorHeap->CreateBufferSRV(device, indexBuffer, sizeof(uint32_t), static_cast<uint32_t>(indices.size()));
		}


		return true;
	}
};