#include "modegen.hpp"

using namespace std::literals;

modegen::meta_parameters::version::version() noexcept : version(0)
{
}

modegen::meta_parameters::version::version(uint64_t v) noexcept : val(v)
{
}

modegen::meta_parameters::version& modegen::meta_parameters::version::operator = (std::uint64_t v) noexcept
{
	val = v;
	return *this;
}

std::string_view modegen::meta_parameters::version::name() const
{
	return "version"sv;
 }

std::string modegen::meta_parameters::version::value() const
{
	return std::to_string(val);
}
