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
public:
	ast_node(Ast n) : ast(std::move(n)) {}

	const Ast& original_node() const { return ast; }

	[[nodiscard]] std::string_view name() const override
	{
		return ast.name;
	}

	std::optional<gen_utils::variable> node_var() const override {return std::nullopt; }
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
};

struct module_node : gen_utils::data_node {
	std::string mod_name;
	module_node(std::string n) : mod_name(std::move(n)) {}

	std::string_view name() const override { return mod_name; }
	std::optional<std::uint64_t> version() const override { return std::nullopt; }
	std::optional<gen_utils::variable> node_var() const override {
		return gen_utils::variable{"name", std::pmr::string(mod_name)}; }
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
};

struct function_node : ast_node<ast::function> {
	function_node(ast::function n) : ast_node(std::move(n)) {}

};

struct fnc_param_node : ast_node<ast::function_parameter> {
	fnc_param_node(ast::function_parameter fp) : ast_node(std::move(fp)) {}
};

struct record_node : ast_node<ast::record> {
	record_node(ast::record r) : ast_node(std::move(r)) {}
};

struct record_field : ast_node<ast::record_item> {
	record_field(ast::record_item i) : ast_node(std::move(i)) {}
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
