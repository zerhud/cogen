#pragma once

#include <filesystem>
#include <cppjson/json.h>

#include "common.hpp"
#include "file_data.hpp"
#include "parser/loader.hpp"

namespace modegen::generation {

class provider {
public:
	virtual ~provider() noexcept =default ;
	virtual parser::loader_ptr target_data(std::string_view name) const =0 ;
	virtual file_data_ptr target_generator(std::string_view name) const =0 ;
	virtual void json_jinja(const cppjson::value& data, const std::filesystem::path& tmpl, const std::optional<std::filesystem::path>& out) const =0 ;
};

} // namespace modegen::generation {
