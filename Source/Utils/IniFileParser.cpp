#include "pch.h"

#include "IniFileParser.hpp"

INIFileParser::INIFileParser() {}
INIFileParser::~INIFileParser() {}

std::vector<INISection> INIFileParser::parse(const std::filesystem::path& path) {
	struct Lexer {
		char ch = ' ';
		uint32_t pos = 0;
	};

	enum INITokenKind {
		INI_EOF,

		LBRACKET,
		RBRACKET,

		SEMI_COL,

		EQUALS,

		VALUE_LITERAL, // abc123
		STRING_LITERAL, // "abc123"
		FLOAT_LITERAL, // 5.0
		INTEGER_LITERAL, // 5
		BOOL_LITERAL, // true || false
	};

	struct INIToken {
		INITokenKind kind;
		std::string value;
	};

	std::vector<INIToken> tokens;

	Lexer lexer;

	auto fileData = readFileBasic(path);

	if (!fileData) {
		LOG_ERROR("Unable to load engine configuration file data");
		return {};
	}

	std::string fileDataString{ fileData };
	free((void*)fileData);

	lexer.ch = fileDataString[lexer.pos];

	auto advance = [&]() {
		if (lexer.pos >= fileDataString.size()) {
			lexer.ch = EOF;
			LOG_ERROR("Out of bounds error when parsing engine configuration file. Pos: {} Size: {}", lexer.pos, fileDataString.size());
			return false;
		}

		lexer.ch = fileDataString[++lexer.pos];

		return true;
	};

	auto isDigit = [](char c) {
		return c >= '0' && c <= '9';
	};

	auto isAlpha = [](char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
	};

	auto isAlphaNum = [&](char c) {
		return isAlpha(c) || isDigit(c);
	};



	while (lexer.ch != '\0') { // while we have not reached the end
		while (lexer.ch == ' ' || lexer.ch == '\n' || lexer.ch == '\t' || lexer.ch == '\r') { // we do not care about whitespace so we always skip it
			if (!advance()) {
				return {};
			}
		}

		if (lexer.ch == ';') { // we skip line comments
			while (lexer.ch != '\n' && lexer.ch != EOF) {
				if (!advance()) {
					return {};
				}
			}
			continue;
		}

		switch (lexer.ch) {
			case '[':
				tokens.push_back(INIToken{ .kind = LBRACKET, .value = "[" });
				if (!advance()) {
					return {};
				}
				break;
			case ']':
				tokens.push_back(INIToken{ .kind = RBRACKET, .value = "]" });
				if (!advance()) {
					return {};
				}
				break;
			case '=':
				tokens.push_back(INIToken{ EQUALS, "=" });
				if (!advance()) {
					return {};
				}
				break;
			case '"':
			{
				if (!advance()) { // go past '"'
					return {};
				}

				uint32_t startPos = lexer.pos;

				while (isAlphaNum(lexer.ch)) {
					if (!advance()) {
						return {};
					}
				}


				auto substr = fileDataString.substr(startPos, (uint64_t)lexer.pos - startPos);

				if (!advance()) { // go past last '"'
					return {};
				}

				tokens.push_back(INIToken{ .kind = STRING_LITERAL, .value = substr });
				break;
			}
			default:
			{ // if it was not a single character
				if (isAlpha(lexer.ch)) {
					uint32_t startPos = lexer.pos;

					while (isAlphaNum(lexer.ch)) {
						if (!advance()) {
							return {};
						}
					}

					auto substr = fileDataString.substr(startPos, (uint64_t)lexer.pos - startPos);

					if (substr == "true" || substr == "false") {
						tokens.push_back(INIToken{ .kind = BOOL_LITERAL, .value = substr == "true" ? "true" : "false" });
						break;
					} else {
						tokens.push_back(INIToken{ .kind = VALUE_LITERAL, .value = substr });
						break;
					}
				} else if (isDigit(lexer.ch)) {
					uint32_t startPos = lexer.pos;

					while (isDigit(lexer.ch)) {
						if (!advance()) {
							return {};
						}
					}

					if (lexer.ch == '.') {
						if (!advance()) { // go past '.'
							return {};
						}

						while (isDigit(lexer.ch)) {
							if (!advance()) {
								return {};
							}
						}

						auto substr = fileDataString.substr(startPos, (uint64_t)lexer.pos - startPos);

						tokens.push_back(INIToken{ .kind = FLOAT_LITERAL, .value = substr });

						break;
					}

					auto substr = fileDataString.substr(startPos, (uint64_t)lexer.pos - startPos);

					tokens.push_back(INIToken{ .kind = INTEGER_LITERAL, .value = substr });
					break;
				} else {
					LOG_ERROR("Invalid token encountered in engine configuration file. The character was: {}", lexer.ch);
					return {};
				}
				break;
			}
		}
	}

	tokens.push_back(INIToken{ .kind = INI_EOF, .value = "\0" }); // we use this for an easy exit condition for the parser.

	struct Parser {
		INIToken currentToken;
		uint32_t pos = 0;
	};

	Parser parser;
	parser.currentToken = tokens[parser.pos];

	auto parserAdvance = [&]() {
		if (parser.pos >= tokens.size()) {
			LOG_ERROR("Out of bounds when attempting to advance parser");
			return false;
		}

		parser.currentToken = tokens[++parser.pos];

		return true;
	};

	auto consume = [&](INITokenKind kind) {
		if (parser.currentToken.kind != kind) {
			LOG_ERROR("Parsing error encountered when parsing the engines configuration file."); // be more specific
			return false;
		}

		if (!parserAdvance()) {
			return false;
		}

		return true;
	};

	while (parser.currentToken.kind != INI_EOF) {
		if (parser.currentToken.kind == INI_EOF) {
			LOG_INFO("Finished parsing engine configuration file");
			break;
		}

		if (parser.currentToken.kind == LBRACKET) {
			INISection section;

			if (!parserAdvance()) {
				return {};
			}

			if (parser.currentToken.kind == VALUE_LITERAL) {
				section.name = _strdup(parser.currentToken.value.c_str());

				if (!parserAdvance()) {
					return {};
				}

				if (!consume(RBRACKET)) {
					LOG_ERROR("Expected a right bracket ']' after the name of the section in the ini file");
					return {};
				}

				while (parser.currentToken.kind != LBRACKET) { // parse keys and values until we find another section

					if (parser.currentToken.kind == INI_EOF) {
						break;
					}

					if (parser.currentToken.kind == VALUE_LITERAL) {
						std::string key = parser.currentToken.value; // important to take a copy here.
						if (!parserAdvance()) {
							return {};
						}

						if (!consume(EQUALS)) {
							LOG_ERROR("Error when parsing engine configuration file. Expected '=' after key value in section {}", section.name);
							return {};
						}

						switch (parser.currentToken.kind) {
							case VALUE_LITERAL:
							{
								INISection::SectionValue val;
								val.type = VALUE_LITERAL;
								val.valueLiteral = _strdup(parser.currentToken.value.c_str());
								section.data[_strdup(key.c_str())] = val;
								if (!parserAdvance()) {
									return {};
								}
								break;
							}
							case STRING_LITERAL:
							{
								INISection::SectionValue val;
								val.type = STRING_LITERAL;
								val.stringValue = _strdup(parser.currentToken.value.c_str());
								section.data[_strdup(key.c_str())] = val;
								if (!parserAdvance()) {
									return {};
								}
								break;
							}
							case FLOAT_LITERAL:
							{
								INISection::SectionValue val;
								val.type = FLOAT_LITERAL;
								val.floatValue = std::stof(parser.currentToken.value);
								section.data[_strdup(key.c_str())] = val;
								if (!parserAdvance()) {
									return {};
								}
								break;
							}
							case INTEGER_LITERAL:
							{
								INISection::SectionValue val;
								val.type = INTEGER_LITERAL;
								val.intValue = std::stoi(parser.currentToken.value);
								section.data[_strdup(key.c_str())] = val;
								if (!parserAdvance()) {
									return {};
								}
								break;
							}
							case BOOL_LITERAL:
							{
								INISection::SectionValue val;
								val.type = BOOL_LITERAL;
								val.boolValue = (parser.currentToken.value == "true" || parser.currentToken.value == "false");
								section.data[_strdup(key.c_str())] = val;
								if (!parserAdvance()) {
									return {};
								}
								break;
							}
						}
					} else {
						LOG_ERROR("Expected a name for the key after the bracket ']'.");
						return {};
					}

				}

				sections.push_back(section);

			} else {
				LOG_ERROR("Expected a name for the section after the bracket '['.");
				return {};
			}

		} else {
			LOG_ERROR("Expected top level construct in configuration file to be a section.");
			return {};
		}
	}

	return sections;
}
