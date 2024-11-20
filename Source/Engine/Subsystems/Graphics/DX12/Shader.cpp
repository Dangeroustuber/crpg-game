#include "pch.h"


#include "Shader.hpp"
#include "Engine/Subsystems/Logger.hpp"

#include <dxcapi.h> // has to be after wrl/client.h

using namespace Microsoft::WRL;

namespace dx12 {
	Shader::Shader(ShaderType type) : type(type) {}

	bool Shader::compile(const std::filesystem::path& path, IDxcCompiler3* compiler, IDxcUtils* utils, std::vector<LPCWSTR>& compileArgs) {
		uint32_t codePage = CP_UTF8;

		ComPtr<IDxcResult> result;
		DxcBuffer buffer = { 0 };

		ComPtr<IDxcBlobEncoding> shaderSourceBlob;
		tryHResult(utils->LoadFile(path.native().data(), &codePage, shaderSourceBlob.GetAddressOf()));

		buffer.Ptr = shaderSourceBlob->GetBufferPointer();
		buffer.Size = shaderSourceBlob->GetBufferSize();
		buffer.Encoding = 0;

		tryHResult(compiler->Compile(&buffer, compileArgs.data(), (uint32_t)compileArgs.size(), nullptr, IID_PPV_ARGS(&result)));

		HRESULT hr;
		tryHResult(result->GetStatus(&hr));

		if (FAILED(hr)) {
			ComPtr<IDxcBlobEncoding> errorsBlob;
			tryHResult(result->GetErrorBuffer(&errorsBlob));
			auto errorStr = reinterpret_cast<const char*>(errorsBlob->GetBufferPointer());

			if (type == ShaderType::Vertex) {
				LOG_ERROR("Failed to compile vertex shader: {}", errorStr);
				return false;
			} else if (type == ShaderType::Pixel) {
				LOG_ERROR("Failed to compile pixel shader: {}", errorStr);
				return false;
			}
		} else {
			tryHResult(result->GetResult(blob.GetAddressOf())); // put the actual shader code in the blob to be used by the PSO
		}

		return true;
	}
}