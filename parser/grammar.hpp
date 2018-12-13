#pragma once

#include <vector>
#include <string_view>
#include "modegen.hpp"

namespace modegen {
modegen::parsed_file parse(std::string_view pdata);
} // namespace modegen
