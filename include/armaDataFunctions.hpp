#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>

using JSON = nlohmann::json;
using String = std::string;
namespace fs = std::filesystem;

namespace arma {
	size_t getSplitCount(const String& s, const int outputSize);
	std::vector<String> splitBySize(const String& s, const int& outputSize);
	void parseOutput(const JSON& json, String& output);
	String getDataPiece(const fs::path filePath, const int outputSize, const int index, const String category = "");
	String getOutput(const fs::path& filePath, const int& outputSize, const String& category);
	String getPieces(const fs::path filePath, const int outputSize, const String category = "");
    std::optional<JSON> recursiveSearch(const JSON &json, const String &category);
}