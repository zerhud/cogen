#pragma once

#include <filesystem>
#include <cppjson/json.h>

namespace modegen {

class json_jinja_generator {
public:
	virtual ~json_jinja_generator() noexcept =default ;
	virtual const json_jinja_generator& operator() (std::filesystem::path tmpl, std::filesystem::path out, const cppjson::value& data) const =0 ;
};

namespace generation {

class jinja_python_generator : public json_jinja_generator {
public:
	jinja_python_generator(std::filesystem::path gen);
	const json_jinja_generator& operator () (std::filesystem::path tmpl, std::filesystem::path out, const cppjson::value& data) const override ;
private:
	std::filesystem::path generator;
};

} // namespace generators
} // namespace modegen
