/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "check.hpp"

#include <memory>
#include <cassert>

#include "helpers.hpp"
#include "errors.h"

const std::string modegen::parser::interface::checker::pdel = ".";

modegen::parser::interface::checker &modegen::parser::interface::checker::operator ()(modegen::parser::interface::parsed_file& finfo)
{
	cur_file = finfo.path;
	auto file_resetter = [this](char*){cur_file.clear();};
	[[maybe_unused]] auto guard = std::unique_ptr<char,decltype(file_resetter)>{nullptr, file_resetter};

	for(auto& mod:finfo) {
		mod.file_name = cur_file;
		check_mod(mod);
		resulting_set.emplace_back(mod);
	}

	return *this;
}

std::vector<modegen::parser::interface::module> modegen::parser::interface::checker::extract_result()
{
	unite_mods(resulting_set);

	std::vector<modegen::parser::interface::module> ret;
	ret.swap(resulting_set);
	return ret;
}

void modegen::parser::interface::checker::unite_mods(std::vector<modegen::parser::interface::module> &mods) const
{
	if(1<mods.size()) {
		auto end = mods.end();
		for(auto beg = mods.begin();beg!=end;++beg) {
			for(auto cmp=std::next(beg);cmp!=end;++cmp) {
				if(beg->name == cmp->name && combine(*beg, *cmp)) {
					std::advance(end,-1);
					if(cmp!=end) *cmp = std::move(*end);
					std::advance(cmp,-1);
				}
			}

			if(beg==end) break;
		}

		if(end != mods.end()) mods.erase(end, mods.end());
	}

	for(auto& mod:mods) {
		check_mod(mod);
		collect_exports(mod);
	}
}

void modegen::parser::interface::checker::check_mod(modegen::parser::interface::module& mod) const
{
	check_using(mod);
	check_names(mod);
	upgrade_cur_min_ver(mod);

	auto check_caller = [this,&mod](auto& v){check(v, mod.name);};
	for(auto& c:mod.content) std::visit(check_caller, c);

	check_version_is_single(mod.meta_params, make_path(mod.name));
	check_deprication_is_single(mod.meta_params, make_path(mod.name));
}

void modegen::parser::interface::checker::check(const modegen::parser::interface::record& r, const std::string& path) const
{
	check_version_is_overmin(r.meta_params, make_path(path, r.name));
	std::vector<std::string> nl;
	for(auto& rm:r.members) nl.emplace_back(rm.name);
	check_names(nl, make_path(path,r.name));
}

void modegen::parser::interface::checker::check(const modegen::parser::interface::function& f, const std::string& path) const
{
	check_version_is_overmin(f.meta_params, make_path(path, f.name));
}

void modegen::parser::interface::checker::check(const modegen::parser::interface::interface& i, const std::string& path) const
{
	check_version_is_overmin(i.meta_params, make_path(path, i.name));

	std::vector<std::string> nl;
	for(auto& fnc:i.mem_funcs) nl.emplace_back(fnc.name);
	check_names(nl, make_path(path,i.name));

	auto old_min_ver = cur_min_ver;
	upgrade_cur_min_ver(i);

	for(auto& fnc:i.mem_funcs) {
		if(!fnc.is_mutable.has_value())
			throw errors::error_info(cur_file, make_path(path,i.name,fnc.name), "all methods must to be const or mutable");
		check(fnc, make_path(path, i.name));
	}

	cur_min_ver = old_min_ver;
}

void modegen::parser::interface::checker::check(modegen::parser::interface::enumeration& e, const std::string& path) const
{
	check_version_is_overmin(e.meta_params, make_path(path, e.name));

	std::vector<std::string> nl;
	for(auto& en:e.elements) nl.emplace_back(en.name);
	check_names(nl, make_path(path,e.name));

	if(e.gen_io) for(auto& en:e.elements) if(en.io.empty()) en.io = en.name;
}

void modegen::parser::interface::checker::check_using(const modegen::parser::interface::module& mod) const
{
	for(auto& u:mod.imports) {
		bool ok = false;
		for(auto& lm:resulting_set) if(lm.name == u.mod_name) ok=true;
		if(!ok) throw errors::error_info(cur_file, mod.name, "no module in using directive " + u.mod_name);
	}
}

void modegen::parser::interface::checker::check_names(const modegen::parser::interface::module& mod) const
{
	std::vector<std::string> nl;
	auto name_collector = [&nl](const auto& e){nl.emplace_back(e.name);};
	for(const auto& c:mod.content) std::visit(name_collector, c);
	check_names(std::move(nl), mod.name);
}

void modegen::parser::interface::checker::check_names(std::vector<std::string> nl, const std::string& path) const
{
	auto check = [&nl](const std::string& v) {for(auto& n:nl)if(n==v)return false; return true;};
	while(nl.size()!=0) {
		auto name = std::move(nl.back());
		if(name.empty()) throw errors::error_info(cur_file, path, "empty name");
		nl.pop_back();
		if(!check(name)) throw errors::error_info(cur_file, path, "duplicate name " + name);
	}
}

void modegen::parser::interface::checker::check_version_is_single(const modegen::parser::interface::meta_parameters::parameter_set& p, const std::string& path) const
{
	bool par_was = false;
	for(auto& par:p.set) {
		if(std::holds_alternative<meta_parameters::version>(par)) {
			if(par_was) throw errors::error_info(cur_file, path, "double version parameter");
			else par_was = true;
		}
	}
}

void modegen::parser::interface::checker::check_version_is_overmin(const modegen::parser::interface::meta_parameters::parameter_set& p, const std::string& path) const
{
	for(auto& par:p) {
		if(auto pv=std::get_if<meta_parameters::version>(&par); pv) {
			if(*pv < cur_min_ver) throw errors::error_info(cur_file, path, "veresion in module must be minimum");
		}
	}
}

void modegen::parser::interface::checker::check_deprication_is_single(const modegen::parser::interface::meta_parameters::parameter_set& p, const std::string& path) const
{
	bool dep_was = false;
	for(auto& par:p.set) {
		if(std::holds_alternative<meta_parameters::deprication>(par)) {
			if(dep_was) throw errors::error_info(cur_file, path, "double deprication parameter");
			else dep_was = true;
		}
	}
}

bool modegen::parser::interface::checker::is_same_ver(const modegen::parser::interface::module& m1, const modegen::parser::interface::module& m2) const
{
	for(auto& p1:m1.meta_params) {
		if(std::holds_alternative<meta_parameters::version>(p1)) {
			for(auto& p2:m2.meta_params) {
				if(std::holds_alternative<meta_parameters::version>(p2)) {
					return std::get<meta_parameters::version>(p1) == std::get<meta_parameters::version>(p2);
				}
			}
		}
	}

	throw errors::error_info(cur_file, m1.name + " | " + m2.name, "one of this modules has no version tag");
}

bool modegen::parser::interface::checker::combine(modegen::parser::interface::module& to, modegen::parser::interface::module& from) const
{
	assert(to.name == from.name);

	if(!is_same_ver(to, from)) return false;

	to.file_name += "; " + from.file_name;
	for(auto& cnt:from.content) to.content.emplace_back(std::move(cnt));
	for(auto& imp:from.imports) to.imports.emplace_back(std::move(imp)); TODO(duplicate imports?)

	//std::optional<meta_parameters::deprication> to_dep_param;
	//for(auto& par:to.meta_params) if(std::holds_alternative<meta_parameters::deprication>(par)) to_dep_param.emplace(std::move(par));

	return true;
}

void modegen::parser::interface::checker::collect_exports(module& mod) const
{
	auto collector = [&mod](const auto& c) {
		if(is_selected(c,module_content_selector::interface)) mod.exports.emplace_back(export_info{c.name, module_content_selector::interface});
		if(is_selected(c,module_content_selector::record)) mod.exports.emplace_back(export_info{c.name, module_content_selector::record});
		if(is_selected(c,module_content_selector::enumeration)) mod.exports.emplace_back(export_info{c.name, module_content_selector::enumeration});
		if(is_selected(c,module_content_selector::function)) mod.exports.emplace_back(export_info{c.name, module_content_selector::function});
	};

	mod.exports.clear();
	for(auto& c:mod.content) std::visit(collector, c);
}

