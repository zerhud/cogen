/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "to_generic_ast.hpp"
#include "meta.hpp"

using namespace std::literals;
using ix3::utils::to_generic_ast;

namespace ix3::utils::details {

std::int64_t splash_version(const ast::meta::version& v)
{
	const auto& a = v.major_v;
	const auto& b = v.minor_v;
	return a >= b ? a * a + a + b : a + b * b;
}

template<typename Ast>
class ast_node : public gen_utils::data_node {
	Ast ast;
protected:
	boost::json::value ast_to_json(
			const ix3::ast::variable_name& obj, const gen_utils::to_json_aspect& asp) const
	{
		(void)asp;
		boost::json::array ret;
		for(auto& n:obj) ret.emplace_back(n);
		return ret;
	}

	boost::json::object ast_to_json(
			const ix3::ast::type& obj, const gen_utils::to_json_aspect& asp) const
	{
		boost::json::object ret;
		ret["type"] = "type"sv;
		ret["name"] = ast_to_json(obj.name, asp);
		boost::json::array& subs=ret["sub"].emplace_array();
		for(auto& sub:obj.sub_types)
			subs.emplace_back(ast_to_json(sub, asp));
		return ret;
	}
public:
	explicit ast_node(Ast n) : ast(std::move(n)) {}

	[[nodiscard]]
	const Ast& original_node() const { return ast; }

	[[nodiscard]] std::string_view name() const override
	{
		return ast.name;
	}

	[[nodiscard]]
	std::optional<gen_utils::variable> node_var() const override
	{
		return std::nullopt;
	}

	[[nodiscard]]
	std::optional<std::uint64_t> version() const override
	{
		if constexpr (requires{Ast::meta_params;}) {
			auto ver = ast::get<ast::meta::version>(original_node().meta_params);
			if(ver) return splash_version(*ver);
		}
		return std::nullopt;
	}
};

struct ix3_root_node : gen_utils::data_node {
	std::string_view name() const override { return "ix3_root"sv; }
	std::optional<std::uint64_t> version() const override { return 0; }
	std::optional<gen_utils::variable> node_var() const override {return std::nullopt; }
	boost::json::object to_json(const gen_utils::to_json_aspect& asp) const override
	{
		(void)asp;
		return boost::json::object{};
	}
};

struct module_node : gen_utils::data_node {
	std::string mod_name;
	module_node(std::string n) : mod_name(std::move(n)) {}

	std::string_view name() const override { return mod_name; }
	std::optional<std::uint64_t> version() const override { return std::nullopt; }
	std::optional<gen_utils::variable> node_var() const override {
		return gen_utils::variable{"name", std::pmr::string(mod_name)}; }
	boost::json::object to_json(const gen_utils::to_json_aspect& asp) const override
	{
		boost::json::object ret;
		ret["name"] = name();
		boost::json::array& content=ret["content"].emplace_array();
		for(auto& child:asp.children(*this))
			content.emplace_back(child->to_json(asp));
		return ret;
	}
};

struct module_version_node : gen_utils::data_node {
	ast::module val;
	std::pmr::string str_val;
	module_version_node(ast::module v) : val(v)
	{
		str_val =
		        std::to_string(val.version.major_v) + '.' +
		        std::to_string(val.version.minor_v);
	}

	std::string_view name() const override { return str_val; }
	std::optional<std::uint64_t> version() const override {
		return splash_version(val.version); }
	std::optional<gen_utils::variable> node_var() const override {
		return gen_utils::variable{"ver", str_val};
	}
	boost::json::object to_json(const gen_utils::to_json_aspect& asp) const override
	{
		boost::json::object ret;
		ret["type"] = "version"sv;
		ret["value"] = str_val;
		boost::json::array& content=ret["content"].emplace_array();
		for(auto& child:asp.children(*this))
			content.emplace_back(child->to_json(asp));
		return ret;
	}
};

struct function_node : ast_node<ast::function> {
	function_node(ast::function n) : ast_node(std::move(n)) {}
	boost::json::object to_json(const gen_utils::to_json_aspect& asp) const override
	{
		boost::json::object ret;
		ret["name"] = name();
		ret["type"] = "function"sv;
		ret["return"] = ast_to_json(original_node().return_type, asp);
		boost::json::array& params=ret["params"].emplace_array();
		for(auto& child:asp.children(*this))
			params.emplace_back(child->to_json(asp));
		return ret;
	}
};

struct fnc_param_node : ast_node<ast::function_parameter> {
	fnc_param_node(ast::function_parameter fp) : ast_node(std::move(fp)) {}
	boost::json::object to_json(const gen_utils::to_json_aspect& asp) const override
	{
		boost::json::object ret;
		ret["name"] = name();
		ret["type"] = "function_parameter"sv;
		ret["param_t"] = ast_to_json(original_node().param_type, asp);
		ret["req"] = original_node().required;
		return ret;
	}
};

struct record_node : ast_node<ast::record> {
	record_node(ast::record r) : ast_node(std::move(r)) {}
	boost::json::object to_json(const gen_utils::to_json_aspect& asp) const override
	{
		boost::json::object ret;
		ret["name"] = name();
		ret["type"] = "record"sv;
		ret["is_exception"] = original_node().use_as_exception;
		boost::json::array& fields=ret["fields"].emplace_array();
		for(auto& f:asp.children(*this))
			fields.emplace_back(f->to_json(asp));
		return ret;
	}
};

struct record_field : ast_node<ast::record_item> {
	record_field(ast::record_item i) : ast_node(std::move(i)) {}
	boost::json::object to_json(const gen_utils::to_json_aspect& asp) const override
	{
		boost::json::object ret;
		ret["name"] = name();
		ret["type"] = "record_item"sv;
		ret["req"] = original_node().is_required;
		ret["param_t"] = ast_to_json(original_node().param_type, asp);
		return ret;
	}
};

} // namespace ix3::utils::details

to_generic_ast::to_generic_ast()
    : result(std::make_shared<details::ix3_root_node>(), "ix3")
{
}

void to_generic_ast::new_mod(ast::module& obj)
{
	using namespace details;
	auto mod_node = std::make_shared<module_node>(std::move(obj.name));
	result.add(result.root(), mod_node);
	parents.emplace_back(mod_node);
	new_mod_ver(obj);
}

void to_generic_ast::new_mod_ver(ast::module& v)
{
	using namespace details;
	auto node = std::make_shared<module_version_node>(v);
	result.add(*parents.back(), node);
	parents.emplace_back(node);
}

void to_generic_ast::on_obj(ast::module &obj)
{
	parents.clear();
	auto mods = result.children(result.root());
	for(auto& m:mods)
		if(m->name()==obj.name) {
			parents.emplace_back(m);
			new_mod_ver(obj);
			return;
		}
	new_mod(obj);
}

void to_generic_ast::on_obj(ast::record& obj)
{
	assert(parents.size()<=2);
	if(parents.size()!=2) parents.resize(2);
	auto par = parents.back();
	parents.emplace_back(std::make_shared<details::record_node>(obj));
	result.add(*par, parents.back());
}

void to_generic_ast::on_obj(ast::record_item& obj)
{
	result.add(*parents.back(), std::make_shared<details::record_field>(obj));
}

void to_generic_ast::on_obj(ast::function& obj)
{
	cur_fnc = std::make_shared<details::function_node>(obj);
	result.add(*parents.back(), cur_fnc);
}

void to_generic_ast::on_obj(ast::function_parameter& obj)
{
	assert(cur_fnc);
	result.add(*cur_fnc, std::make_shared<details::fnc_param_node>(obj));
}

gen_utils::tree to_generic_ast::operator()(std::vector<ast::module> mods)
{
	for(auto& mod:mods) trav_module(mod, trav_direction::paret_first);
	return result;
}
