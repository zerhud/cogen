/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "meta_parameters.hpp"

using namespace std::literals;

modegen::parser::interface::meta_parameters::version::version() noexcept : version(0, 0)
{
}

modegen::parser::interface::meta_parameters::version::version(std::uint64_t m, std::uint64_t i) noexcept
    : major_v(m)
    , minor_v(i)
{
}

std::string_view modegen::parser::interface::meta_parameters::version::name() const
{
	return "version"sv;
}

std::string modegen::parser::interface::meta_parameters::version::value(std::string_view sep) const
{
	return std::to_string(major_v) + std::string(sep) + std::to_string(minor_v);
}

std::string modegen::parser::interface::meta_parameters::version::value() const
{
	return std::to_string(major_v) + "." + std::to_string(minor_v);
}

bool modegen::parser::interface::meta_parameters::version::add(const modegen::parser::interface::meta_parameter& /*other*/)
{
	return false;
}

std::string_view modegen::parser::interface::meta_parameters::documentation::name() const
{
	return "documentation"sv;
}

std::string modegen::parser::interface::meta_parameters::documentation::value() const
{
	return body;
}

bool modegen::parser::interface::meta_parameters::documentation::add(const modegen::parser::interface::meta_parameter& other)
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

void modegen::parser::interface::meta_parameters::parameter_set::push_back(modegen::parser::interface::meta_parameters::parameter_set::parameter_type p)
{
	auto name_checker = [](const auto& p1, const auto& p2){ return p1.name() == p2.name(); };
	auto adder = [](auto& p1, const auto& p2){ return p1.add(p2); };

	if(!set.empty() && std::visit(name_checker, set.back(), p) && std::visit(adder, set.back(), p)) return;
	set.emplace_back(std::move(p));
}

std::string_view modegen::parser::interface::meta_parameters::deprication::name() const
{
	return "depricated"sv;
}

std::string modegen::parser::interface::meta_parameters::deprication::value() const
{
	return message;
}

bool modegen::parser::interface::meta_parameters::deprication::add(const modegen::parser::interface::meta_parameter& other)
{
	// we cannt have two deprication attribute
	return false;
}
