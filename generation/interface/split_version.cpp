/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "split_version.hpp"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

#include "parser/interface/meta_parameters.hpp"

namespace mg = modegen::generation;
namespace mi = modegen::parser::interface;
namespace mgi = modegen::generation::interface;
using mi::meta_parameters::version;

mgi::split_version::split_version(bool dry_run)
	: dry(dry_run)
{
}

std::vector<mi::module>& mgi::split_version::operator () (std::vector<mi::module>& mods)
{
	result.clear();
	if(dry) return mods;

	for(auto& m:mods) split_mod(std::move(m));
	result.swap(mods);

	return mods;
}

void mgi::split_version::split_mod(mi::module mod)
{
	current_mod = result.size();
	prepare_modules(mod);

	for(;current_mod<result.size();++current_mod) {
		for(auto& c:mod.content) insert_content(c);
	}
}

void mgi::split_version::insert_content(const mi::module_content& cnt)
{
	auto cnt_ver = mi::get<version>(cnt);
	version cur_ver = mi::get_version(result[current_mod]);

	const bool for_cur_ver = !cnt_ver || *cnt_ver <= cur_ver;
	if(!for_cur_ver) return ;

	mi::module& cur_mod = result[current_mod];
	auto cnt_pos = std::find_if(std::begin(cur_mod.content), std::end(cur_mod.content),
			[&cnt](const mi::module_content& mc){
				return mi::name(mc) == mi::name(cnt);
			});

	if(cnt_pos == std::end(cur_mod.content)) {
		cur_mod.content.emplace_back(mi::copy(cnt, mi::copy_method::meta));
		cnt_pos = cur_mod.content.end()-1;
	}

	if(std::holds_alternative<mi::record>(cnt)) deep_copy(std::get<mi::record>(cnt), std::get<mi::record>(*cnt_pos));
	if(std::holds_alternative<mi::interface>(cnt)) deep_copy(std::get<mi::interface>(cnt), std::get<mi::interface>(*cnt_pos));
}

void mgi::split_version::deep_copy(const mi::record& from, mi::record& to)
{
	version cur_ver = mi::get_version(result[current_mod]);
	for(const mi::record_item& item:from.members) {
		auto item_ver = mi::get<version>(item);
		if(!item_ver || *item_ver <= cur_ver)
			to.members.emplace_back(item);
	}
}

void mgi::split_version::deep_copy(const mi::interface& from, mi::interface& to)
{
	version cur_ver = mi::get_version(result[current_mod]);
	for(const auto& item:from.mem_funcs) {
		auto item_ver = mi::get<version>(item);
		if(!item_ver || *item_ver <= cur_ver)
			to.mem_funcs.emplace_back(item);
	}
	for(const auto& item:from.constructors) {
		auto item_ver = mi::get<version>(item);
		if(!item_ver || *item_ver <= cur_ver)
			to.constructors.emplace_back(item);
	}
}

void mgi::split_version::prepare_modules(const parser::interface::module& mod)
{
	auto empty_mod = mi::copy(mod, mi::copy_method::meta);

	std::vector<version> vlist = all_versions(mod);
	for(auto& v:vlist) {
		mi::set(result.emplace_back(empty_mod), v);
	}
}

std::vector<version> mgi::split_version::all_versions(const mi::module& mod) const
{
	std::vector<version> ret;
	ret.reserve(mod.content.size() + 1 );
	ret.emplace_back(mi::get_version(mod));

	auto add_ver = [&ret](const auto& list) {
		for(const auto& item:list) {
			auto ver = mi::get<version>(item);
			if(ver) ret.emplace_back(*ver);
		}
	};

	add_ver(mod.content);
	for(auto& c:mod.content) {
		if(std::holds_alternative<mi::interface>(c)) {
			const mi::interface& i = std::get<mi::interface>(c);
			add_ver(i.mem_funcs);
			add_ver(i.constructors);
		}
		else if(std::holds_alternative<mi::record>(c)) {
			const mi::record& r = std::get<mi::record>(c);
			add_ver(r.members);
		}
	}

	boost::unique(boost::sort(ret));
	return ret;
}

