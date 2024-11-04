#include "Utils.hpp"
#include "Engine/Subsystems/Logger.hpp"

#include <comdef.h>
#include <string>
#include <codecvt>

const char* readFileBasic(const std::filesystem::path& path) {
	auto filePath = path.string();
	FILE* inFile = fopen(filePath.data(), "rb");

	if (!inFile) {
		LOG_ERROR("Unable to open file for reading. Filepath given was {}", filePath);
		return nullptr;
	}

	// Get file size
	int32_t fseekRet = fseek(inFile, 0, SEEK_END);

	if (fseekRet != 0) {
		LOG_ERROR("Failed when seeking for the size of the file. Filepath given was {}", filePath);
		return nullptr;
	}

	int32_t size = ftell(inFile);

	if (size == -1L) {
		LOG_ERROR("Failed in ftell(). Filepath given was {}", filePath);
		return nullptr;
	}

	rewind(inFile);

	char* buffer = (char*)malloc((size_t)size + 1);

	if (!buffer) {
		int32_t fcloseRet = fclose(inFile);

		if (fcloseRet == EOF) {
			LOG_ERROR("Unable to close file. File was: {}", filePath);
		}

		return nullptr;
	}

	size_t readSize = fread(buffer, 1, size, inFile);
	int32_t fcloseRet = fclose(inFile);

	if (fcloseRet == EOF) {
		LOG_ERROR("Unable to close file. File was: {}", filePath);
		free(buffer);
		return nullptr;
	}

	if (readSize != size) {
		LOG_ERROR("The amount of data read was not equal to the expected size. File was: {}", filePath);
		free(buffer);
		return nullptr;
	}

	buffer[size] = '\0';

	return buffer;
}

void tryHResult(HRESULT hr) {
	if (FAILED(hr)) {
		_com_error err{ hr };
		std::wstring wstr{ err.ErrorMessage() };

		int32_t size = WideCharToMultiByte(CP_UTF8, 0, &wstr.front(), (int32_t)wstr.size(), NULL, 0, NULL, NULL);
		std::string message(size, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr.front(), (int32_t)wstr.size(), &message.front(), size, NULL, NULL);

		LOG_FATAL("HRESULT did not return a valid success code. Message: {}", message.c_str());
	}
}
