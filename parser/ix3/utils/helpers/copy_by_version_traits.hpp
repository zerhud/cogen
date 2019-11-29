/*************************************************************************
 * copyright © 2019 hudyaev alexy <hudyaev.alexy@gmail.com>
 * this file is part of modegen.
 * distributed under the gnu affero general public license.
 * see accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/file.hpp"

namespace ix3::utils::helpers {

struct module_split_traits {
	static std::optional<ast::meta::version> get_ver(const ast::module& mod);
	void clear_content(ast::module& mod);
	void update_version(ast::module& mod, const ast::meta::version& ver);

	template<typename T>
	void add_child(ast::module& mod, const T& child)
	{
		mod.content.emplace_back(child);
	}

	template<typename T>
	T* find_object(ast::module& mod, const T& child)
	{
		for(auto& cnt:mod.content) {
			if(cnt.var.type() != typeid(T)) continue;
			T& found = boost::get<T>(cnt.var);
			if(found.name == child.name) return &found;
		}

		return nullptr;
	}
};

struct record_split_traits {
	static std::optional<ast::meta::version> get_ver(const ast::record& mod);
	void clear_content(ast::record& mod);
	void update_version(ast::record& mod, const ast::meta::version& ver);

	template<typename T>
	void add_child(ast::record& mod, const T& child)
	{
		mod.members.emplace_back(child);
	}

	template<typename T>
	T* find_object(ast::record& mod, const T& child)
	{
		for(auto& cnt:mod.members)
			if(cnt.name == child.name) return &cnt;
		return nullptr;
	}
};

struct interface_split_traits {
	static std::optional<ast::meta::version> get_ver(const ast::interface& mod);
	void clear_content(ast::interface& mod);
	void update_version(ast::interface& mod, const ast::meta::version& ver);

	void add_child(ast::interface& obj, const ast::constructor& child);
	void add_child(ast::interface& obj, const ast::function& child);

	ast::function*    find_object(ast::interface& obj, const ast::function& child);
	ast::constructor* find_object(ast::interface& obj, const ast::constructor& child);
};

} // namespace ix3::utils::helpers

