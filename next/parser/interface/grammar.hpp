#pragma once

#include <vector>
#include <string_view>
#include "modegen.hpp"

namespace modegen::parser::interface {
modegen::parsed_file parse(std::string_view pdata);
} // namespace modegen::parser::interface

