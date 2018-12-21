#pragma once

#include <filesystem>
#include "modegen.hpp"

namespace modegen {

std::filesystem::path get_self_dir();

bool is_selected(const module_content& cnt, module_content_selector s);

template<typename Stream>
Stream& operator << (Stream& s, const meta_parameter& p)
{
	s << p.name() << ": " << p.value();
	return s;
}

template<typename Stream>
Stream& operator << (Stream& out, const module_content_selector s)
{
	if(s == module_content_selector::all) out << "all";
	if(s == module_content_selector::enumeration) out << "enum";
	if(s == module_content_selector::interface) out << "interface";
	if(s == module_content_selector::record) out << "record";
	if(s == module_content_selector::function) out << "function";
	return out;
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

template<typename P, typename MType>
std::optional<P> get(const MType& mobj)
{
	return get<P>(mobj.meta_params);
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
