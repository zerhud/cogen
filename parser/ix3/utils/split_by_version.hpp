/*************************************************************************
 * copyright © 2019 hudyaev alexy <hudyaev.alexy@gmail.com>
 * this file is part of modegen.
 * distributed under the gnu affero general public license.
 * see accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "meta.hpp"
#include "evaluator.hpp"
#include "ast/file.hpp"
#include "helpers/copy_by_version.hpp"

namespace ix3::utils {

class split_by_version : public evaluator {
public:
	split_by_version(bool dry_run = false);
private:
	void eval_module(ast::module& mod) override ;
	std::vector<ast::module>& extract_result() override ;

	void on_obj(ast::module& obj) override ;
	void on_obj(ast::record& obj)  override ;
	void on_obj(ast::function& obj) override    { (void)obj; }
	void on_obj(ast::interface& obj) override   { (void)obj; }
	void on_obj(ast::enumeration& obj) override { (void)obj; }
	void on_obj(ast::record_item& obj) override ;
	void on_obj(ast::constructor& obj) override { (void)obj; }

	void on_obj(ast::enum_element& obj) override       { (void)obj; }
	void on_obj(ast::function_parameter& obj) override { (void)obj; }

	void on_obj(ast::meta::set& obj) override           { (void)obj; }
	void on_obj(ast::meta::oapi& obj) override          { (void)obj; }
	void on_obj(ast::meta::version& obj) override       { (void)obj; }
	void on_obj(ast::meta::depricated& obj) override    { (void)obj; }
	void on_obj(ast::meta::documentation& obj) override { (void)obj; }

	struct versioned_modules {
		std::vector<ast::module> mods;

		versioned_modules(const ast::module& mod);

		std::string_view name() const ;

		ast::module& max_mod() ;
		void sort();

		void add(const ast::module& mod);

		template<typename T>
		void add(const T& obj)
		{
			auto adder = [this](auto& obj) {
				using ast::meta::version;
				auto obj_ver = ast::get<version>(obj.meta_params);
				replace_after(obj, obj_ver);
			};
			boost::apply_visitor(adder, obj.var);
		}

		template<typename T>
		void replace_after(const T& obj, std::optional<ast::meta::version> ver)
		{
			if(ver && max_mod().version < *ver) {
				auto& mm = mods.emplace_back(max_mod());
				mm.version = *ver;
				sort();
			}

			for(auto& m:mods) {
				if(ver && m.version < *ver) continue;

				T* found;
				found = find_object(m, obj);
				if(found) *found = obj;
				else m.content.emplace_back(obj);
			}
		}

		template<typename T>
		T* find_object(ast::module& mod, const T& obj)
		{
			for(auto& cnt:mod.content) {
				if(cnt.var.type() != typeid(T)) continue;
				T& found = boost::get<T>(cnt.var);
				if(found.name == obj.name) return &found;
			}

			return nullptr;
		}
	};

	bool dry = false;
	typedef helpers::copy_by_version<ast::module, helpers::module_split_traits> cpy_by_ver_t;
	std::vector<cpy_by_ver_t> result;
	std::vector<ast::module> finished_result;
};

} // namespace ix3::utils

