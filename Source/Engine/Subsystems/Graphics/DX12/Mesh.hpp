#pragma once

#include "pch.h"

#include "UploadHeap.hpp"
#include "DescriptorHeap.hpp"
#include "GraphicsContext.hpp"

#include <SimpleMath.h>

namespace dx12 {
    struct Mesh {
        struct MeshVertex {
            DirectX::SimpleMath::Vector3 position;
            DirectX::SimpleMath::Vector4 color;
        };

        bool initialize(GraphicsContext* graphicsContext, UploadHeap* uploadHeap, DescriptorHeap* descriptorHeap);

        std::vector<MeshVertex> vertices;

        std::vector<uint32_t> indices;

        ID3D12Resource* vertexBuffer;
        ID3D12Resource* indexBuffer;

        int32_t vertexBufferIndex = -1;
        int32_t indexBufferIndex = -1;
        bool hasIndices = false;
    };
};