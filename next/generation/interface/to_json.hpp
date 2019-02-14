#pragma once

#include <vector>
#include <cppjson/json.h>
#include <boost/ptr_container/ptr_vector.hpp>

#include "common.hpp"
#include "parser/interface/modegen.hpp"

namespace modegen::generation::interface {

class to_json_aspect {
public:
	virtual ~to_json_aspect() noexcept =default ;
	virtual void as_object(cppjson::value& jval, const modegen::parser::interface::module& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::parser::interface::function& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::parser::interface::enumeration& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::parser::interface::interface& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::parser::interface::record& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::parser::interface::record_item& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::parser::interface::type& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::parser::interface::func_param& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::parser::interface::constructor_fnc& obj) {(void)obj; (void)jval;}
	virtual void as_object(cppjson::value& jval, const modegen::parser::interface::meta_parameters::version& obj) {(void)obj; (void)jval;}
};

class to_json {
	typedef std::vector<std::unique_ptr<to_json_aspect>> aspect_ptr_vector;
public:
	to_json();
	to_json(std::unique_ptr<to_json_aspect> gen_aspect);
	to_json(aspect_ptr_vector gen_aspects);
	to_json& operator () (std::vector<modegen::parser::interface::module>& m) ;
	operator std::string () const ;
	operator cppjson::value () const ;
private:
	void generate() ;

	cppjson::value as_object(const modegen::parser::interface::module& obj) const ;
	cppjson::value as_object(const modegen::parser::interface::function& obj) const ;
	cppjson::value as_object(const modegen::parser::interface::enumeration& obj) const ;
	cppjson::value as_object(const modegen::parser::interface::interface& obj) const ;
	cppjson::value as_object(const modegen::parser::interface::record& obj) const ;
	cppjson::value as_object(const modegen::parser::interface::record_item& obj) const ;
	cppjson::value as_object(const modegen::parser::interface::type& obj) const ;
	cppjson::value as_object(const modegen::parser::interface::func_param& obj) const ;
	cppjson::value as_object(const modegen::parser::interface::constructor_fnc& obj) const ;
	cppjson::value as_object(const modegen::parser::interface::meta_parameters::version& obj) const ;

	void add_meta(cppjson::value& val, const modegen::parser::interface::meta_parameters::parameter_set& params) const ;

	template<typename T>
	void applay_asp(cppjson::value& val, const T& obj) const
	{
		for(const auto& a:gen_asps) a->as_object(val, obj);
	}

	template<typename P, typename O>
	std::optional<P> extract(const O& o) const
	{
		for(auto& p:o) {
			if(auto pv=std::get_if<P>(&p); pv) return *pv;
		}

		return std::nullopt;
	}

	std::vector<modegen::parser::interface::module> mods;
	cppjson::value result;

	aspect_ptr_vector gen_asps;
};

} // namespace modegen::generation::interface
