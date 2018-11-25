#pragma once

#include "modegen.hpp"

namespace modegen {

template<typename Stream>
Stream& operator << (Stream& s, const meta_parameter& p)
{
	s << p.name() << ": " << p.value();
	return s;
}

namespace meta_parameters {
bool operator < (const version& left, const version& right);
bool operator <= (const version& left, const version& right);
bool operator == (const version& left, const version& right);
} // namespace modegen

template<typename P>
std::optional<P> get(const meta_parameters::parameter_set& set)
{
	for(auto& par:set) if(std::holds_alternative<P>(par)) return std::get<P>(par);
	return std::nullopt;
}

template<typename P>
bool has(const meta_parameters::parameter_set& set)
{
	for(auto& p:set) if(std::holds_alternative<P>(p)) return true;
	return false;
}

template<typename P, typename MType>
bool has(const MType& mobj)
{
	return has<P>(mobj.meta_params);
}

template<typename P>
bool has(const module_content& c)
{
	auto ch = [](const auto& c) {return has<P>(c);};
	return std::visit(ch, c);
}

template<typename P>
void set(meta_parameters::parameter_set& set, P p)
{
	for(auto& par:set) {
		if(std::holds_alternative<P>(par)) {
			std::get<P>(par) = std::move(p);
			return;
		}
	}

	set.set.emplace_back(std::move(p));
}

} // namespace modegen
