/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "split_by_version.hpp"

using namespace ix3::utils;
using ix3::ast::meta::version;

split_by_version::split_by_version(bool dry_run) : dry(dry_run) {}

void split_by_version::eval_module(ast::module& mod)
{
	if(dry) result.emplace_back(std::move(mod));
	else trav_module(mod, trav_direction::child_first);
}

std::vector<ix3::ast::module>& split_by_version::extract_result()
{
	finished_result.clear();
	for(auto& r:result) for(auto& m:r.mods()) finished_result.emplace_back(m);
	return finished_result;
}

void split_by_version::on_obj(ast::module& obj)
{
	cpy_by_ver_t* current = nullptr;
	for(auto& r:result) {
		if(r.name()==obj.name) {
			current = &r;
			break;
		}
	}

	if(current) current->add(obj);
	else current = &result.emplace_back(obj);

	auto adder = [this,current](auto& obj) {
		using ast::meta::version;
		auto obj_ver = ast::get<version>(obj.meta_params);
		current->replace_after(obj, obj_ver);
	};

	for(auto& cnt:obj.content) {
		boost::apply_visitor(adder, cnt.var);
	}
}

void split_by_version::on_obj(ast::record& obj)
{
}

void split_by_version::on_obj(ast::record_item& obj)
{
	auto obj_ver = ast::get<version>(obj.meta_params);
	if(!obj_ver) return;

	auto* par = parent<ast::record>(0);
	assert(par);

	auto par_ver = ast::get<version>(par->meta_params);
	if(par_ver && *par_ver < *obj_ver) {
		//TODO: 1. remove this item somehow
		//      2. copy record, and set version == obj_ver
	}
}

ix3::utils::split_by_version::versioned_modules::versioned_modules(const ix3::ast::module& mod)
	: mods{ mod }
{
	mods[0].content.clear();
}

std::string_view ix3::utils::split_by_version::versioned_modules::name() const
{
	assert( 0 < mods.size() );
	return mods[0].name;
}

ix3::ast::module& ix3::utils::split_by_version::versioned_modules::max_mod()
{
	assert( 0 < mods.size() );
	return mods.back();
}

void ix3::utils::split_by_version::versioned_modules::sort()
{
	std::sort(mods.begin(),mods.end(),
		[](const ast::module& left, const ast::module& right){
			assert( &left == &right || left.name == right.name );
			assert( &left == &right || left.version != right.version );
			return left.version < right.version;
		});
}

void ix3::utils::split_by_version::versioned_modules::add(const ast::module& mod)
{
	assert( name() == mod.name );
	for(auto& m:mods) if(m.version==mod.version) return;
	mods.emplace_back(mod).content.clear();
	sort();
}

