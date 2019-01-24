#pragma once

#include <filesystem>
#include <cppjson/json.h>

namespace modegen::generation {

class jinja_file_generator {
public:
	jinja_file_generator(std::filesystem::path gen);
	void operator () (std::filesystem::path tmpl, std::filesystem::path out, const cppjson::value& data) const ;
private:
	std::filesystem::path generator;
};

} // namespace modegen::generators

