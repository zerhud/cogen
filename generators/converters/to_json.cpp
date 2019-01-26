#include "to_json.h"

#include <sstream>
#include <functional>

#include "helpers.hpp"

namespace pl = std::placeholders;


modegen::cvt::to_json::to_json()
{
}

modegen::cvt::to_json::to_json(std::unique_ptr<modegen::cvt::to_json_aspect> gen_aspect)
{
	gen_asps.push_back(std::move(gen_aspect));
}

modegen::cvt::to_json::to_json(boost::ptr_vector<modegen::cvt::to_json_aspect> gen_aspects)
	: gen_asps(std::move(gen_aspects))
{
}

modegen::cvt::to_json& modegen::cvt::to_json::operator () (std::vector<modegen::module>& m)
{
	mods = std::move(m);
	generate();
	m = std::move(mods);
	return *this;
}

modegen::cvt::to_json::operator std::string () const
{
	std::stringstream out;
	out << result;
	return out.str();
}

modegen::cvt::to_json::operator cppjson::value () const
{
	return result;
}

void modegen::cvt::to_json::generate()
{
	for(std::size_t i=0;i<mods.size();++i) result["mods"][i] = as_object(mods[i]);
}

cppjson::value modegen::cvt::to_json::as_object(const modegen::module& obj) const
{
	cppjson::value ret;
	ret["name"] = obj.name;
	ret["type"] = "module";

	add_meta(ret, obj.meta_params);

	std::size_t s = 0;
	auto make_obj = [this](const auto& obj){return as_object(obj);};
	for(std::size_t i=0;i<obj.content.size();++i) {
		ret["content"][i] = std::visit(make_obj, obj.content[i]);
	}

	for(std::size_t i=0;i<obj.imports.size();++i) ret["imports"][i] = obj.imports[i].mod_name;

	applay_asp(ret, obj);

	return ret;
}

cppjson::value modegen::cvt::to_json::as_object(const modegen::function& obj) const
{
	cppjson::value ret;
	ret["name"] = obj.name;
	ret["type"] = "function";
	ret["ret_type"] = as_object(obj.return_type);

	if(obj.is_mutable) ret["mutable"] = *obj.is_mutable;
	else ret["mutable"] = cppjson::null{};

	if(obj.is_static) ret["static"] = *obj.is_static;
	else ret["static"] = cppjson::null{};

	ret["params"] = cppjson::array();
	for(std::size_t i=0;i<obj.func_params.size();++i) ret["params"][i]=as_object(obj.func_params[i]);

	add_meta(ret, obj.meta_params);

	applay_asp(ret, obj);

	return ret;
}

cppjson::value modegen::cvt::to_json::as_object(const modegen::enumeration& obj) const
{
	cppjson::value ret;
	add_meta(ret, obj.meta_params);
	ret["name"] = obj.name;
	ret["type"] = "enumeration";
	ret["use_bitmask"] = obj.use_bitmask;

	bool gen_io_required = false;
	for(std::size_t i=0;i<obj.elements.size();++i) {
		ret["members"][i]["name"] = obj.elements[i].name;
		ret["members"][i]["output"] = obj.elements[i].io;
		gen_io_required = gen_io_required || !obj.elements[i].io.empty();
	}

	if(gen_io_required) ret["gen_io"] = true;
	else ret["gen_io"] = obj.gen_io;

	applay_asp(ret, obj);

	return ret;
}

cppjson::value modegen::cvt::to_json::as_object(const modegen::interface& obj) const
{
	cppjson::value ret;
	ret["name"] = obj.name;
	ret["type"] = "interface";
	ret["invert"] = obj.realization_in_client;
	add_meta(ret, obj.meta_params);

	for(std::size_t i=0;i<obj.mem_funcs.size();++i)
		ret["members"][i] = as_object(obj.mem_funcs[i]);

	for(std::size_t i=0;i<obj.constructors.size();++i)
		ret["constructors"][i] = as_object(obj.constructors[i]);

	applay_asp(ret, obj);

	return ret;
}

cppjson::value modegen::cvt::to_json::as_object(const modegen::record& obj) const
{
	cppjson::value ret;
	ret["name"] = obj.name;
	ret["type"] = "record";
	add_meta(ret, obj.meta_params);

	for(std::size_t i=0; i<obj.members.size(); ++i) {
		ret["members"][i] = as_object(obj.members[i]);
	}

	applay_asp(ret, obj);

	return ret;
}

cppjson::value modegen::cvt::to_json::as_object(const modegen::record_item& obj) const
{
	cppjson::value ret;
	ret["name"]=obj.name;
	ret["type"]="record_item";
	ret["par_type"]=as_object(obj.param_type);
	add_meta(ret, obj.meta_params);

	applay_asp(ret, obj);

	return ret;
}

cppjson::value modegen::cvt::to_json::as_object(const modegen::type& obj) const
{
	cppjson::value ret;

	ret["type"] = "type";

	ret["name"] = obj.name;
	if(obj.sub_types.empty()) ret["sub"] = cppjson::null{};
	for(std::size_t i=0;i<obj.sub_types.size();++i)
		ret["sub"][i] = as_object(obj.sub_types[i]);

	applay_asp(ret, obj);

	return ret;
}

cppjson::value modegen::cvt::to_json::as_object(const modegen::func_param& obj) const
{
	cppjson::value ret;
	ret["name"] = obj.name;
	ret["type"] = "func_param";
	ret["par_type"] = as_object(obj.param_type);

	applay_asp(ret, obj);
	return ret;
}

cppjson::value modegen::cvt::to_json::as_object(const modegen::constructor_fnc& obj) const
{
	cppjson::value ret;
	ret["type"] = "constructor";
	ret["params"] = cppjson::array();
	for(std::size_t i=0;i<obj.func_params.size();++i) ret["params"][i]=as_object(obj.func_params[i]);

	applay_asp(ret, obj);
	return ret;
}

cppjson::value modegen::cvt::to_json::as_object(const modegen::meta_parameters::version& obj) const
{
	cppjson::value ret;
	ret["major"] = obj.major_v;
	ret["minor"] = obj.minor_v;

	applay_asp(ret, obj);
	return ret;
}

void modegen::cvt::to_json::add_meta(cppjson::value& val, const modegen::meta_parameters::parameter_set& params) const
{
	auto ver = extract<modegen::meta_parameters::version>(params);
	if(!ver) val["v"] = cppjson::null{};
	else val["v"] = as_object(*ver);

	auto docs = extract<modegen::meta_parameters::documentation>(params);
	if(docs) val["docs"] = docs->body;

	auto dep = extract<modegen::meta_parameters::deprication>(params);
	if(dep) {
		val["depricated"]["since"] = as_object(dep->since);
		val["depricated"]["message"] = dep->message;
	}
}

