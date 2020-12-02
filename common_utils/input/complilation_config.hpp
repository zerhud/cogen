#pragma once

#include "tree.hpp"

namespace gen_utils::compilation_config_impl {

class compilation_config : public gen_utils::compilation_config {
public:
	[[nodiscard]]
	compiler compiler_name() const override ;

	[[nodiscard]]
	std::string_view value(std::string_view key) const override ;
};

} // gen_utils::compilation_config_impl