#pragma once

#include "tree.hpp"

namespace gen_utils::compilation_config_impl {

class compilation_config : public gen_utils::compilation_config {
	compiler config_type ;
public:
	explicit compilation_config( compiler cmp) :config_type(cmp){} ;
	[[nodiscard]]
	compiler compiler_name() const override ;
	[[nodiscard]]
	std::string_view value(std::string_view key) const override ;
	name_conversion naming() const override ;
};

} // gen_utils::compilation_config_impl
