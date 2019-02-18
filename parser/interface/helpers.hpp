#pragma once

#include "modegen.hpp"

namespace modegen::parser::interface {
bool is_selected(const module_content& cnt, module_content_selector s);
module_content_selector which_selected(const module_content& cnt);

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

module_content_selector from_string(std::string_view v);

bool operator == (const using_directive& left, const using_directive& right);
bool operator == (const using_directive& left, std::string_view right);
bool operator == (const export_info& left, std::string_view right);

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

template<typename MType>
meta_parameters::version get_version(const MType& mobj)
{
	assert( has<meta_parameters::version>(mobj) );
	return *get<meta_parameters::version>(mobj);
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

enum class copy_method{ deep, meta };
module copy(const module& mod, copy_method method=copy_method::meta);
} // namespace modegen::parser::interface
