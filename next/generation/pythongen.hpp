#pragma once

#include "config.hpp"
#include FILESYSTEM
#include <cppjson/json.h>

namespace modegen {
namespace generation {

class json_jinja_generator {
public:
	virtual ~json_jinja_generator() noexcept =default ;
	virtual const json_jinja_generator& operator() (FS::path tmpl, FS::path out, const cppjson::value& data) const =0 ;
};


class jinja_python_generator : public json_jinja_generator {
public:
	jinja_python_generator(FS::path gen);
	const json_jinja_generator& operator () (FS::path tmpl, FS::path out, const cppjson::value& data) const override ;
private:
	FS::path generator;
};

} // namespace generators
} // namespace modegen
