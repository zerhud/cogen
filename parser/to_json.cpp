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
	add_meta(ret, obj.meta_params);
	return ret;
}

cppjson::value modegen::converters::to_json::as_object(const modegen::record& obj) const
{
	cppjson::value ret;
	ret["name"] = obj.name;
	ret["type"] = "record";
	add_meta(ret, obj.meta_params);
	return ret;
}

void modegen::converters::to_json::add_meta(cppjson::value& val, const modegen::meta_parameters::parameter_set& params) const
{
	auto ver = extract<modegen::meta_parameters::version>(params);
	if(ver) val["v"] = ver->val;
	else val["v"] = cppjson::null{};

	auto docs = extract<modegen::meta_parameters::documentation>(params);
	if(docs) val["docs"] = docs->body;
}
