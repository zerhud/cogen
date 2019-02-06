#pragma once

#include <cppjson/json.h>
#include "common.hpp"

namespace modegen::generation {

class provider {
public:
	virtual ~provider() noexcept =default ;
	virtual void json_jinja(const cppjson::value& data, const std::filesystem::path& tmpl) const =0 ;
};

} // namespace modegen::generation {
