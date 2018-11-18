#include "to_json.h"

#include <sstream>

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

cppjson::value modegen::converters::to_json::as_object(const modegen::module& mod) const
{
	cppjson::value ret;
	ret["name"] = mod.name;

	auto mod_ver = extract<modegen::meta_parameters::version>(mod);
	if(mod_ver) ret["v"] = mod_ver->val;
	else ret["v"] = cppjson::null{};

	auto mod_docs = extract<modegen::meta_parameters::documentation>(mod);
	if(mod_docs) ret["docs"] = mod_docs->body;

	return ret;
}
