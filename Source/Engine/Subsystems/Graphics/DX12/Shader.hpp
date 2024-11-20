#pragma once

#include "pch.h"

#include <dxcapi.h> // has to be after wrl/client.h

namespace dx12 {
	enum ShaderType {
		Vertex,
		Pixel,
	};

	struct Shader {
		Shader(ShaderType type);

		bool compile(const std::filesystem::path& path, IDxcCompiler3* compiler, IDxcUtils* utils, std::vector<LPCWSTR>& compileArgs);

		Microsoft::WRL::ComPtr<IDxcBlob> blob;
		ShaderType type;
	};
}