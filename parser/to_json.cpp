#include "to_json.h"

#include <sstream>
#include <functional>

namespace pl = std::placeholders;

modegen::converters::to_json::to_json(const std::vector<modegen::module>& mods)
{
	for(auto& m:mods) result.emplace_back(as_object(m));
}

modegen::converters::to_json::operator cppjson::value() const
{
	return result;
}

modegen::converters::to_json::operator std::string() const
{
	std::stringstream out;
	out << result;
	return out.str();
}

cppjson::value modegen::converters::to_json::as_object(const modegen::module& obj) const
{
	cppjson::value ret;
	ret["name"] = obj.name;
	ret["type"] = "module";

	add_meta(ret, obj.meta_params);

	auto make_obj = [this](const auto& obj){return as_object(obj);};
	for(std::size_t i=0;i<obj.content.size();++i) {
		ret["content"][i] = std::visit(make_obj, obj.content[i]);
	}

	return ret;
}

cppjson::value modegen::converters::to_json::as_object(const modegen::function& obj) const
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

	return ret;
}

cppjson::value modegen::converters::to_json::as_object(const modegen::enumeration& obj) const
{
	cppjson::value ret;
	add_meta(ret, obj.meta_params);
	ret["name"] = obj.name;
	ret["type"] = "enumeration";
	ret["gen_io"] = obj.gen_io;
	ret["use_bitmask"] = obj.use_bitmask;

	for(std::size_t i=0;i<obj.elements.size();++i) {
		ret["members"][i]["name"] = obj.elements[i].name;
		ret["members"][i]["output"] = obj.elements[i].io;
	}

	return ret;
}

cppjson::value modegen::converters::to_json::as_object(const modegen::interface& obj) const
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

	return ret;
}

cppjson::value modegen::converters::to_json::as_object(const modegen::record& obj) const
{
	cppjson::value ret;
	ret["name"] = obj.name;
	ret["type"] = "record";
	add_meta(ret, obj.meta_params);

	for(std::size_t i=0; i<obj.members.size(); ++i) {
		ret["members"][i] = as_object(obj.members[i]);
	}

	return ret;
}

cppjson::value modegen::converters::to_json::as_object(const modegen::type& obj) const
{
	cppjson::value ret;
	ret["name"] = obj.name;
	ret["type"] = "type";
	ret["modificator"] = obj.modificator;
	return ret;
}

cppjson::value modegen::converters::to_json::as_object(const modegen::func_param& obj) const
{
	cppjson::value ret;
	ret["name"] = obj.name;
	ret["type"] = "func_param";
	ret["par_type"] = as_object(obj.param_type);
	return ret;
}

cppjson::value modegen::converters::to_json::as_object(const modegen::constructor_fnc& obj) const
{
	cppjson::value ret;
	ret["type"] = "constructor";
	ret["params"] = cppjson::array();
	for(std::size_t i=0;i<obj.func_params.size();++i) ret["params"][i]=as_object(obj.func_params[i]);
	return ret;
}

void modegen::converters::to_json::add_meta(cppjson::value& val, const modegen::meta_parameters::parameter_set& params) const
{
	auto ver = extract<modegen::meta_parameters::version>(params);
	if(!ver) val["v"] = cppjson::null{};
	else {
		val["v"]["major"] = ver->major_v;
		val["v"]["minor"] = ver->minor_v;
	}

	auto docs = extract<modegen::meta_parameters::documentation>(params);
	if(docs) val["docs"] = docs->body;
}
