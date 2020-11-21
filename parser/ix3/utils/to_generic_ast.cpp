/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "to_generic_ast.hpp"
#include "meta.hpp"

#include "details/ix3_node_base.hpp"

using namespace std::literals;
using ix3::utils::to_generic_ast;
using ix3::utils::ix3_manager;

namespace ix3::utils::details {

std::int64_t splash_version(const ast::meta::version& v)
{
	const auto& a = v.major_v;
	const auto& b = v.minor_v;
	return a >= b ? a * a + a + b : a + b * b;
}

template<typename Ast>
class ast_node : public ix3_node_base {
	Ast ast;
protected:
	boost::json::value ast_to_json(const ix3::ast::variable_name& obj) const {
		boost::json::array ret;
		for (auto& n:obj) ret.emplace_back(n);
		return ret;
	}

	boost::json::object ast_to_json(const ix3::ast::type& obj) const {
		boost::json::object ret;
		ret["type"] = "type"sv;
		ret["name"] = ast_to_json(obj.name);
		boost::json::array& subs = ret["subs"].emplace_array();
		for (auto& sub:obj.sub_types)
			subs.emplace_back(ast_to_json(sub));
		return ret;
	}

public:
	explicit ast_node(Ast n) : ast(std::move(n)) {}

	[[nodiscard]]
	const Ast& original_node() const { return ast; }

	[[nodiscard]] std::string_view name() const override {
		return ast.name;
	}

	[[nodiscard]]
	std::optional<gen_utils::variable> node_var() const override {
		return std::nullopt;
	}

	[[nodiscard]]
	std::optional<std::uint64_t> version() const override {
		if constexpr (requires{ Ast::meta_params; }) {
			auto ver = ast::get<ast::meta::version>(original_node().meta_params);
			if (ver) return splash_version(*ver);
		}
		return std::nullopt;
	}

	boost::json::object make_json(const compilation_context& ctx) const override
	{
		boost::json::object ret;
		ret["orig_name"] = name();
		return ret;
	}
};

struct ix3_root_node : ix3_node_base {
	std::string_view name() const override { return "ix3_root"sv; }
	std::optional<std::uint64_t> version() const override { return 0; }
	boost::json::object make_json(const compilation_context& ctx) const override
	{
		boost::json::object ret;
		ret["name"] = "ix3";
		boost::json::array& cnt = ret["mods"].emplace_array();
		for(auto& child:ctx.children(*this))
			cnt.emplace_back(child->make_json(ctx));
		ctx.compiling_aspect().aspect(*this, ret);
		return ret;
	}
};

struct module_node : ix3_node_base {
	std::string mod_name;

	module_node(std::string n) : mod_name(std::move(n)) {}

	std::string_view name() const override { return mod_name; }

	std::optional<std::uint64_t> version() const override { return std::nullopt; }

	std::optional<gen_utils::variable> node_var() const override {
		return gen_utils::variable{"mod", std::pmr::string(mod_name)};
	}

	boost::json::object make_json(const compilation_context& ctx) const override
	{
		boost::json::object ret;
		ret["name"] = name();
		boost::json::array& content=ret["content"].emplace_array();
		for(auto& child:ctx.children(*this))
			content.emplace_back(child->make_json(ctx));
		ctx.compiling_aspect().aspect(*this, ret);
		return ret;
	}
};

struct module_version_node : ix3_node_base {
	ast::module val;
	std::pmr::string str_val;
	module_version_node(ast::module v) : val(std::move(v))
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
	boost::json::object make_json(const compilation_context& ctx) const override
	{
		boost::json::object ret;
		ret["type"] = "version"sv;
		ret["value"] = str_val;
		boost::json::array& content=ret["content"].emplace_array();
		for(auto& child:ctx.children(*this))
			content.emplace_back(child->make_json(ctx));
		ctx.compiling_aspect().aspect(*this, ret);
		return ret;
	}
};

struct function_node : ast_node<ast::function> {
	function_node(ast::function n) : ast_node(std::move(n)) {}
	boost::json::object make_json(const compilation_context& ctx) const override
	{
		boost::json::object ret = ast_node::make_json(ctx);
		ret["type"] = "function"sv;
		ret["return"] = ast_to_json(original_node().return_type);
		boost::json::array& params=ret["params"].emplace_array();
		for(auto& child:ctx.children(*this))
			params.emplace_back(child->make_json(ctx));
		return ret;
	}
};

struct fnc_param_node : ast_node<ast::function_parameter> {
	fnc_param_node(ast::function_parameter fp) : ast_node(std::move(fp)) {}
	boost::json::object make_json(const compilation_context& ctx) const override
	{
		boost::json::object ret = ast_node::make_json(ctx);
		ret["type"] = "function_parameter"sv;
		ret["param_t"] = ast_to_json(original_node().param_type);
		ret["req"] = original_node().required;
		return ret;
	}
};

struct record_node : ast_node<ast::record> {
	record_node(ast::record r) : ast_node(std::move(r)) {}
	boost::json::object make_json(const compilation_context& ctx) const override
	{
		boost::json::object ret = ast_node::make_json(ctx);
		ret["type"] = "record"sv;
		ret["is_exception"] = original_node().use_as_exception;
		boost::json::array& fields=ret["fields"].emplace_array();
		for(auto& f:ctx.children(*this))
			fields.emplace_back(f->make_json(ctx));
		return ret;
	}
};

struct record_field : ast_node<ast::record_item> {
	record_field(ast::record_item i) : ast_node(std::move(i)) {}
	boost::json::object make_json(const compilation_context& ctx) const override
	{
		boost::json::object ret = ast_node::make_json(ctx);
		ret["type"] = "record_item"sv;
		ret["req"] = original_node().is_required;
		ret["param_t"] = ast_to_json(original_node().param_type);
		return ret;
	}
};

struct cpp_compiler : ix3_compiler {
	const gen_utils::compilation_config* config;

	cpp_compiler(const gen_utils::compilation_config* c) : config(c) {}

	void aspect(const ix3_node_base& node, boost::json::object& res) const override{}
	void aspect(const module_node& node, boost::json::object& res) const override {}
	void aspect(const module_version_node& node, boost::json::object& res) const override
	{
		res["name"] =
				node.val.name + "_v" +
				std::to_string(node.val.version.major_v) + '_' +
				std::to_string(node.val.version.minor_v);
	}
	void aspect(const function_node& node, boost::json::object& res) const override {}
	void aspect(const record_node& node, boost::json::object& res) const override {}
};

} // namespace ix3::utils::details

std::string_view ix3_manager::id() const { return "ix3"sv; }
boost::json::value ix3_manager::to_json(
		const gen_utils::compilation_config& cfg,
		const gen_utils::tree& container) const
{
	using details::ix3_root_node;
	assert(dynamic_cast<const ix3_root_node*>(&container.root()) != nullptr);
	const auto* root = static_cast<const ix3_root_node*>(&container.root());
	assert(cfg.compiler_name()==gen_utils::compiler::cpp);
	details::cpp_compiler compiler{&cfg};
	details::compilation_context ctx( &container, &compiler );
	return root->make_json(ctx);
}

to_generic_ast::to_generic_ast()
    : ix3_owner(std::make_shared<ix3_manager>())
    , result(std::make_shared<details::ix3_root_node>(), ix3_owner)
{
}

void to_generic_ast::new_mod(ast::module& obj)
{
	using namespace details;
	auto mod_node = std::make_shared<module_node>(obj.name);
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
