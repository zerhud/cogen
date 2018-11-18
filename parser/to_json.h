#pragma once

#include <vector>
#include <cppjson/json.h>

#include "modegen.hpp"

namespace modegen {
namespace converters {

class to_json final {
public:
	to_json(const std::vector<modegen::module>& mods);
	operator std::string () const ;
	operator cppjson::value () const ;
private:
	cppjson::value as_object(const modegen::module& obj) const ;
	cppjson::value as_object(const modegen::function& obj) const ;
	cppjson::value as_object(const modegen::enumeration& obj) const ;
	cppjson::value as_object(const modegen::interface& obj) const ;
	cppjson::value as_object(const modegen::record& obj) const ;
	cppjson::value as_object(const modegen::type& obj) const ;
	cppjson::value as_object(const modegen::func_param& obj) const ;

	void add_meta(cppjson::value& val, const modegen::meta_parameters::parameter_set& params) const ;

	template<typename P, typename O>
	std::optional<P> extract(const O& o) const
	{
		for(auto& p:o) {
			if(auto pv=std::get_if<P>(&p); pv) return *pv;
		}

		return std::nullopt;
	}

	cppjson::array result;
};

template<typename S>
S& operator << (S& out, const to_json& o)
{
	out << (cppjson::value)o ;
	return out;
}

} // namespace converters
} // namespace modegen
