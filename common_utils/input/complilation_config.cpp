#include "complilation_config.hpp"

gen_utils::compiler gen_utils::compilation_config_impl::compilation_config::compiler_name() const
{
	return gen_utils::compiler::cpp;
}

std::string_view gen_utils::compilation_config_impl::compilation_config::value(std::string_view key) const
{
	return std::string_view();
}
