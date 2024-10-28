#include "armaDataFunctions.hpp"
#include <fstream>

namespace arma {

	size_t getSplitCount(const String& s, const int outputSize) {
		return ceil((s.length() / outputSize) + 1);
	}

	std::vector<String> splitBySize(const String& s, const int& outputSize) {
		size_t pieces = getSplitCount(s, outputSize);
		std::vector<String> vec;
		vec.reserve(pieces);

		for (unsigned int i = 0; i < pieces; ++i) {
			String ss = s.substr((i) * outputSize, outputSize);
			vec.push_back(ss);
		}

		return vec;
	}

	void parseOutput(const JSON& json, String& output) {
		bool isFirst = true; //need this flag as first() and back() have undefined behaviour on empty objects or arrays
		for(const auto& [key, val] : json.items()) {
			if (!isFirst) {
				output += ", ";
			}
			else {
				isFirst = false;
			}
			if (val.is_object()) {

				output +=  "[\"" + key + "\", " + "createHashMapFromArray[";

				if (!val.empty()) {
					parseOutput(val, output);
				}

				output += "]]";

			}else if(val.is_array()) {

				if (json.is_object()) {
					output += "[\"" + key + "\", [";
				}
				else {
					output +=  "[";
				}

				if (!val.empty()) {
					parseOutput(val, output);
				}

				if (json.is_object()) {
					output += "]]";
				}
				else {
					output +=  "]";
				}		
			} else {
				if (json.is_object()) {
					output += "[\"" + key + "\", " + val.dump() + "]";
				}
				else {
					output +=  val.dump();
				}
			}
		}
	}

	std::optional<JSON> recursiveSearch(const JSON& json, const String& category) {
		for(const auto& [key, val] : json.items()) {
			if(key == category) {
				return val;
			}
			else if(val.is_object() || val.is_array()) {
				JSON j = recursiveSearch(val, category).value_or(JSON{});
				if(!j.empty()) return j;		
			}
		}
		return {};
	}
	String getOutputString(const fs::path& filePath, const int& outputSize, const String& category) {
		String output = "";

		JSON json;
		try {
			std::ifstream jsonFile(filePath);
			if(jsonFile) {
				json = JSON::parse(jsonFile);
			}
			else {
				throw std::runtime_error(filePath.string() + " not found");
			}
			if(!category.empty()) {
				JSON data = recursiveSearch(json, category).value_or(JSON{});
				if(data.empty())
					return "createHashMap";
				json = data;
			}
			output += "createHashMapFromArray[";
			parseOutput(json, output);
			output += "]";
		}
		catch (const std::exception& e) {
			return e.what();
		}

		return output;
	}

	String getOutputJSONString(const fs::path& filePath, const int& outputSize) {
		std::ifstream jsonFile(filePath);
		JSON json;

		if(jsonFile) {
			json = JSON::parse(jsonFile);
		}
		else {
			throw std::runtime_error(filePath.string() + " not found");
		}
		
		return json.dump();
	}

	String getDataPiece(fs::path filePath, const int outputSize, const int index, const String category) {
		String output = getOutputJSONString(filePath, outputSize);
		auto split = splitBySize(output, outputSize);
		try {
			return split.at(index);
		}
		catch (const std::exception& e) {
			return e.what();
		}
	}

	String getPieces(const fs::path filePath, const int outputSize, const String category) {
		String output = getOutputJSONString(filePath, outputSize);
		auto pieces = splitBySize(output, outputSize).size();
		return std::to_string(pieces);
	}

}