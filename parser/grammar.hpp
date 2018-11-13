#pragma once

#include <vector>
#include <string_view>
#include "modegen.hpp"

namespace modegen {
std::vector<modegen::module> parse(std::string_view pdata);
} // namespace modegen
