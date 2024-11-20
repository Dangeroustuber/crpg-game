#pragma once

#include "pch.h"
#include "Mesh.hpp"

#include <unordered_map>

struct GraphicsContext;

namespace dx12 {
	using MeshHandle = uint32_t;

	struct ResourceManager {
		MeshHandle loadMesh(const std::filesystem::path& path, GraphicsContext* graphicsContext, UploadHeap* uploadHeap, DescriptorHeap* descriptorHeap);
		Mesh* getMesh(MeshHandle handle);

		// Remove heap allocation
		std::unordered_map<uint32_t, Mesh*> meshes;
		static uint32_t meshId;
	};
};