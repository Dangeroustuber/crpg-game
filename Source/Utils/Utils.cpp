#include "Utils.hpp"
#include "Engine/Subsystems/Logger.hpp"

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
