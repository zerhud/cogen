#pragma once

#include <vector>
#include <string>
#include "parser/modegen.hpp"

namespace modegen {
namespace generators {

enum class name_conversion{underscore, camel_case, title_case, as_is};

std::string_view to_string(name_conversion c);
name_conversion from_string(std::string_view n);

std::vector<std::string> split_name(const std::string& name);

std::string convert(const std::string& name, name_conversion c);

void convert(std::vector<module>& mods, name_conversion c);

} // namespace generators
} // namespace modegen
