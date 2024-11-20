#pragma once
#include "pch.h"

#define MAKE_NON_COPYABLE(name)			   \
	name(const name&) = delete;			   \
	name& operator=(const name&) = delete; \
	name(name&&) = delete;				   \
	name& operator=(name&&) = delete;		   

const char* readFileBasic(const std::filesystem::path& path);

void tryHResult(HRESULT hr);

uint32_t alignTo256Bytes(uint32_t baseSize);