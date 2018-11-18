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
	cppjson::value as_object(const modegen::module& mod) const ;

	template<typename P, typename O>
	std::optional<P> extract(const O& o) const
	{
		for(auto& p:o.meta_params) {
			if(auto pv=std::get_if<P>(&p); pv) return *pv;
		}

		return std::nullopt;
	}

	cppjson::array result;
};

} // namespace converters
} // namespace modegen
