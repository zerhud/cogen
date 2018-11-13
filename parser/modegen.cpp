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

bool modegen::meta_parameters::version::add(const modegen::meta_parameter& /*other*/)
{
	return false;
}

std::string_view modegen::meta_parameters::documentation::name() const
{
	return "documentation"sv;
}

std::string modegen::meta_parameters::documentation::value() const
{
	return body;
}

bool modegen::meta_parameters::documentation::add(const modegen::meta_parameter& other)
{
	auto* v = dynamic_cast<const documentation*>(&other);
	if(v) {
		bool just_spaces = true;
		for(auto& c:v->body) if(c!=' ') just_spaces = false;

		if(just_spaces) body += '\n';
		else body += " " + v->body;
	}

	return v;
}

void modegen::meta_parameters::parameter_set::push_back(modegen::meta_parameters::parameter_set::parameter_type p)
{
	auto name_checker = [](const auto& p1, const auto& p2){ return p1.name() == p2.name(); };
	auto adder = [](auto& p1, const auto& p2){ return p1.add(p2); };

	if(!set.empty() && std::visit(name_checker, set.back(), p) && std::visit(adder, set.back(), p)) return;
	set.emplace_back(std::move(p));
}

std::string_view modegen::meta_parameters::deprication::name() const
{
	return "depricated"sv;
}

std::string modegen::meta_parameters::deprication::value() const
{
	return message;
}

bool modegen::meta_parameters::deprication::add(const modegen::meta_parameter& other)
{
	// we cannt have two deprication attribute
	return false;
}
