/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "to_json.hpp"

#include <sstream>
#include <functional>

#include "parser/interface/helpers.hpp"

namespace pl = std::placeholders;
namespace mg = modegen::generation::interface;
namespace mi = modegen::parser::interface;


mg::to_json::to_json()
{
}

mg::to_json::to_json(std::unique_ptr<mg::to_json_aspect> gen_aspect)
{
	gen_asps.emplace_back(std::move(gen_aspect));
}

mg::to_json::to_json(mg::to_json::aspect_ptr_vector gen_aspects)
	: gen_asps(std::move(gen_aspects))
{
}

nlohmann::json mg::to_json::operator () (std::vector<modegen::parser::interface::module>& m)
{
	mods = std::move(m);
	generate();
	m = std::move(mods);
	return result;
}

std::string mg::to_json::str() const
{
	std::stringstream out;
	out << result;
	return out.str();
}

void mg::to_json::generate()
{
	for(std::size_t i=0;i<mods.size();++i) result["mods"][i] = as_object(mods[i]);
}

nlohmann::json mg::to_json::as_object(const modegen::parser::interface::module& obj) const
{
	nlohmann::json ret;
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

nlohmann::json mg::to_json::as_object(const modegen::parser::interface::function& obj) const
{
	nlohmann::json ret;
	ret["name"] = obj.name;
	ret["type"] = "function";
	ret["ret_type"] = as_object(obj.return_type);

	if(obj.is_mutable) ret["mutable"] = *obj.is_mutable;
	else ret["mutable"] = nullptr;

	if(obj.is_static) ret["static"] = *obj.is_static;
	else ret["static"] = nullptr;

	for(auto& fp:obj.func_params) ret["params"].push_back(as_object(fp));

	add_meta(ret, obj.meta_params);

	applay_asp(ret, obj);

	return ret;
}

nlohmann::json mg::to_json::as_object(const modegen::parser::interface::enumeration& obj) const
{
	nlohmann::json ret;
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

nlohmann::json mg::to_json::as_object(const modegen::parser::interface::interface& obj) const
{
	nlohmann::json ret;
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

nlohmann::json mg::to_json::as_object(const modegen::parser::interface::record& obj) const
{
	nlohmann::json ret;
	ret["name"] = obj.name;
	ret["type"] = "record";
	add_meta(ret, obj.meta_params);

	for(std::size_t i=0; i<obj.members.size(); ++i) {
		ret["members"][i] = as_object(obj.members[i]);
	}

	applay_asp(ret, obj);

	return ret;
}

nlohmann::json mg::to_json::as_object(const modegen::parser::interface::record_item& obj) const
{
	nlohmann::json ret;
	ret["name"]=obj.name;
	ret["type"]="record_item";
	ret["par_type"]=as_object(obj.param_type);
	add_meta(ret, obj.meta_params);

	applay_asp(ret, obj);

	return ret;
}

nlohmann::json mg::to_json::as_object(const modegen::parser::interface::type& obj) const
{
	nlohmann::json ret;

	ret["type"] = "type";

	ret["name"] = obj.name;
	if(obj.sub_types.empty()) ret["sub"] = nullptr;
	for(auto& st:obj.sub_types) ret["sub"].push_back(as_object(st));

	applay_asp(ret, obj);

	return ret;
}

nlohmann::json mg::to_json::as_object(const modegen::parser::interface::func_param& obj) const
{
	nlohmann::json ret;
	ret["name"] = obj.name;
	ret["type"] = "func_param";
	ret["par_type"] = as_object(obj.param_type);

	applay_asp(ret, obj);
	return ret;
}

nlohmann::json mg::to_json::as_object(const modegen::parser::interface::constructor_fnc& obj) const
{
	nlohmann::json ret;
	ret["type"] = "constructor";
	for(auto& fp:obj.func_params) ret["params"].push_back(as_object(fp));

	applay_asp(ret, obj);
	return ret;
}

nlohmann::json mg::to_json::as_object(const modegen::parser::interface::meta_parameters::version& obj) const
{
	nlohmann::json ret;
	ret["major"] = obj.major_v;
	ret["minor"] = obj.minor_v;

	applay_asp(ret, obj);
	return ret;
}

void mg::to_json::add_meta(nlohmann::json& val, const modegen::parser::interface::meta_parameters::parameter_set& params) const
{
	using namespace modegen::parser::interface::meta_parameters;

	auto ver = extract<version>(params);
	if(!ver) val["v"] = nullptr;
	else val["v"] = as_object(*ver);

	auto docs = extract<documentation>(params);
	if(docs) val["docs"] = docs->body;

	auto dep = extract<deprication>(params);
	if(dep) {
		val["depricated"]["since"] = as_object(dep->since);
		val["depricated"]["message"] = dep->message;
	}
}

