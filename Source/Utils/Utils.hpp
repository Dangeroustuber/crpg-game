#pragma once

#include "Engine/Subsystems/Logger.hpp"

#include <filesystem>
#include <comdef.h>

#define MAKE_NON_COPYABLE(name)			   \
	name(const name&) = delete;			   \
	name& operator=(const name&) = delete; \
	name(name&&) = delete;				   \
	name& operator=(name&&) = delete;		   

const char* readFileBasic(const std::filesystem::path& path);

void tryHResult(HRESULT hr);