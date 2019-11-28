/*************************************************************************
 * copyright © 2019 hudyaev alexy <hudyaev.alexy@gmail.com>
 * this file is part of modegen.
 * distributed under the gnu affero general public license.
 * see accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <string>
#include "ast/file.hpp"
#include "../meta.hpp"

namespace ix3::utils::helpers {

template<typename T, typename ObjectTraits>
class copy_by_version {
	std::vector<T> objs;
	ObjectTraits traits;
public:
	copy_by_version(T obj, ObjectTraits ot=ObjectTraits{})
	    : objs{obj}
	    , traits(std::move(ot))
	{
		traits.clear_content(objs[0]);
	}

	std::vector<T>& mods() { return objs; }

	std::vector<T> extract_result()
	{
		std::vector<T> ret;
		ret.swap(objs);
		return ret;
	}

	std::string_view name() const
	{
		assert(0 < objs.size());
		return objs[0].name;
	}

	T& max_obj()
	{
		assert( 0 < objs.size() );
		return objs.back();
	}

	void sort()
	{
		using ast::meta::version;
		std::sort(objs.begin(),objs.end(),
			[](const T& left, const T& right){
				auto lv = ObjectTraits::get_ver(left);
				auto rv = ObjectTraits::get_ver(right);

				assert( &left == &right || left.name == right.name );
				assert( &left == &right || lv != rv );

				return lv < rv ;
			});
	}

	void add(const T& obj)
	{
		assert( name() == obj.name );
		auto obj_ver = traits.get_ver(obj);
		for(auto& o:objs) if(traits.get_ver(o) == obj_ver) return;
		traits.clear_content(objs.emplace_back(obj));
		sort();
	}

	template<typename Child>
	void replace_after(const Child& child, std::optional<ast::meta::version> ver)
	{
		using ast::meta::version;
		if(ver && *traits.get_ver(max_obj()) < *ver) {
			auto& nobj = objs.emplace_back(max_obj());
			traits.update_version(nobj, *ver);
			sort();
		}

		for(auto& obj:objs) {
			if(ver && *traits.get_ver(obj) < *ver) continue;

			Child* found = traits.find_object(obj, child);
			if(found) *found = child;
			else traits.add_child(obj, child);
		}
	}
};

struct module_split_traits {
	static std::optional<ast::meta::version> get_ver(const ast::module& mod) { return mod.version; }
	void clear_content(ast::module& mod) { mod.content.clear(); }
	void update_version(ast::module& mod, const ast::meta::version& ver) { mod.version = ver; }
	template<typename T>
	void add_child(ast::module& mod, const T& child) { mod.content.emplace_back(child); }
	template<typename T>
	T* find_object(ast::module& mod, const T& child) {
		for(auto& cnt:mod.content) {
			if(cnt.var.type() != typeid(T)) continue;
			T& found = boost::get<T>(cnt.var);
			if(found.name == child.name) return &found;
		}

		return nullptr;
	}
};

} // namespace ix3::utils::helpers
