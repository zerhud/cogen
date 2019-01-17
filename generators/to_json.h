#pragma once

#include <vector>
#include <cppjson/json.h>

#include "parser/modegen.hpp"

namespace modegen {
namespace converters {

class to_json_aspect {
public:
	virtual ~to_json_aspect() noexcept =default ;
	virtual void final_object(cppjson::value& jval) { (void)jval; }
	virtual void as_object(cppjson::value& jval, const modegen::module& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::function& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::enumeration& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::interface& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::record& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::record_item& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::type& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::func_param& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::constructor_fnc& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::meta_parameters::version& obj) {(void)obj; (void)jval;}
};

class to_json final {
public:
	to_json(const std::vector<modegen::module>& m);
	to_json(const std::vector<modegen::module>& m, to_json_aspect& asp);
	operator std::string () ;
	operator cppjson::value () ;
private:
	void generate() ;

	cppjson::value as_object(const modegen::module& obj) const ;
	cppjson::value as_object(const modegen::function& obj) const ;
	cppjson::value as_object(const modegen::enumeration& obj) const ;
	cppjson::value as_object(const modegen::interface& obj) const ;
	cppjson::value as_object(const modegen::record& obj) const ;
	cppjson::value as_object(const modegen::record_item& obj) const ;
	cppjson::value as_object(const modegen::type& obj) const ;
	cppjson::value as_object(const modegen::func_param& obj) const ;
	cppjson::value as_object(const modegen::constructor_fnc& obj) const ;
	cppjson::value as_object(const modegen::meta_parameters::version& obj) const ;

	void add_meta(cppjson::value& val, const modegen::meta_parameters::parameter_set& params) const ;

	template<typename P, typename O>
	std::optional<P> extract(const O& o) const
	{
		for(auto& p:o) {
			if(auto pv=std::get_if<P>(&p); pv) return *pv;
		}

		return std::nullopt;
	}

	const std::vector<modegen::module>& mods;
	cppjson::value result;

	to_json_aspect* aspect=nullptr;
};

template<typename S>
S& operator << (S& out, const to_json& o)
{
	out << (cppjson::value)o ;
	return out;
}

} // namespace converters
} // namespace modegen
