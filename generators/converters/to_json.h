#pragma once

#include <vector>
#include <cppjson/json.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include "../converters.hpp"

namespace modegen::cvt {

class to_json_aspect {
public:
	virtual ~to_json_aspect() noexcept =default ;
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

class to_json {
public:
	to_json();
	to_json(std::unique_ptr<to_json_aspect> gen_aspect);
	to_json(boost::ptr_vector<to_json_aspect> gen_aspects);
	to_json& operator () (std::vector<module>& m) ;
	operator std::string () const ;
	operator cppjson::value () const ;
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

	template<typename T>
	void applay_asp(cppjson::value& val, const T& obj) const
	{
		for(auto a:gen_asps) a.as_object(val, obj);
	}

	template<typename P, typename O>
	std::optional<P> extract(const O& o) const
	{
		for(auto& p:o) {
			if(auto pv=std::get_if<P>(&p); pv) return *pv;
		}

		return std::nullopt;
	}

	std::vector<modegen::module> mods;
	cppjson::value result;

	boost::ptr_vector<to_json_aspect> gen_asps;
};

} // namespace modegen::cvt


