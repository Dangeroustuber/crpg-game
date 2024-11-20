#pragma once
#include "pch.h"

#include <unordered_map>

// this only parses sections and basic key values in the sections.
// does handle semicolon starting single line comments

struct INISection {
	const char* name;

	struct SectionValue {
		int32_t type;

		union {
			int32_t intValue;
			float floatValue;
			const char* valueLiteral; // abc123
			const char* stringValue; // "abc123"
			bool boolValue;
		};

	};

	std::unordered_map<const char*, SectionValue> data;
};

struct INIFileParser {
	INIFileParser();
	~INIFileParser();

	std::vector<INISection> sections;

	std::vector<INISection> parse(const std::filesystem::path& path);
};