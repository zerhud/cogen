#pragma once

#include <vector>
#include <string>

namespace modegen {
namespace generators {

enum class name_conversion{underscore, camel_case, title_case};

std::vector<std::string> split_name(const std::string& name);

std::string convert(const std::string& name, name_conversion);

} // namespace generators
} // namespace modegen
