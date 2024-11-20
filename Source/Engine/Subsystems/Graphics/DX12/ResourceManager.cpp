#include "ResourceManager.hpp"
#include "Mesh.hpp"
#include "../Source/Utils/Random.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace dx12 {
	uint32_t ResourceManager::meshId = 0;

	MeshHandle ResourceManager::loadMesh(const std::filesystem::path& path, GraphicsContext* graphicsContext, UploadHeap* uploadHeap, DescriptorHeap* descriptorHeap) {
		MeshHandle handle{ ++ResourceManager::meshId };
		
		Assimp::Importer Importer;

		const aiScene* scene = Importer.ReadFile(path.string().c_str(), aiProcess_Triangulate);

		if (scene) {
			if (scene->HasMeshes()) {
				auto mesh = new Mesh;
				auto aiMesh = scene->mMeshes[0];
				
				for (uint32_t i = 0; i < aiMesh->mNumVertices; ++i) {
					const auto& vertex = aiMesh->mVertices[i];
					DirectX::SimpleMath::Vector4 color;

					if (aiMesh->HasVertexColors(0)) {
						auto& col = aiMesh->mColors[0][i];
						color = { col.r, col.g, col.b, col.a };
					} else {
						color = { Random::randomFloat(), Random::randomFloat(), Random::randomFloat(), 1.0f };
					}
					mesh->vertices.push_back({{vertex.x, vertex.y, vertex.z},color});
				}
				
				if (aiMesh->HasFaces()) {
					mesh->hasIndices = true;
					for (uint32_t i = 0; i  < aiMesh->mNumFaces; i++) {
						auto& face = aiMesh->mFaces[i];
						for (uint32_t j = 0; j < face.mNumIndices; j++) {
							mesh->indices.push_back(face.mIndices[j]);
						}
					}
				}
				
				mesh->initialize(graphicsContext, uploadHeap, descriptorHeap);
				meshes[handle] = mesh;
			} else {
				LOG_ERROR("Unable to load mesh correctly. No mesh present in assimp scene");
				return {};
			}
		} else {
			LOG_ERROR("Unable to load mesh.");
			return {};
		}

		return handle;
	}

	Mesh* ResourceManager::getMesh(MeshHandle handle) {
		if (!handle) {
			return nullptr;
		}

		if (auto it = meshes.find(handle); it != meshes.end()) {
			return (*it).second;
		} else {
			return nullptr;
		}
	}
};