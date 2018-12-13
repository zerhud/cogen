#include "check.h"

#include <memory>
#include <cassert>

#include "helpers.hpp"

const std::string modegen::checker::pdel = ".";

modegen::error_info::error_info(std::string f, std::string p, std::string w)
    : std::runtime_error("in " + f + " " + p + ": " + w)
    , file(std::move(f))
    , path(std::move(p))
    , what(std::move(w))
{
}

modegen::checker &modegen::checker::operator ()(modegen::parsed_file& finfo)
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

std::vector<modegen::module> modegen::checker::extract_result()
{
	std::vector<modegen::module> ret;
	ret.swap(resulting_set);
	unite_mods(ret);
	return ret;
}

void modegen::checker::unite_mods(std::vector<modegen::module> &mods) const
{
	if(1<mods.size()) {
		auto end = mods.end();
		for(auto beg = mods.begin();beg!=end;++beg) {
			for(auto cmp=beg+1;cmp!=end;++cmp) {
				if(beg->name == cmp->name && combine(*beg, *cmp)) {
					if(--end == cmp--) break;;
					*cmp = std::move(*end);
				}
			}

			if(beg==end) break;
		}

		if(end != mods.end()) mods.erase(end, mods.end());
	}

	for(auto& mod:mods) check_mod(mod);
}

void modegen::checker::check_mod(modegen::module& mod) const
{
	std::vector<std::string> nl;
	auto name_collector = [&nl](const auto& e){nl.emplace_back(e.name);};
	for(auto& c:mod.content) std::visit(name_collector, c);
	check_names(std::move(nl), mod.name);

	upgrade_cur_min_ver(mod);

	auto check_caller = [this,&mod](auto& v){check(v, mod.name);};
	for(auto& c:mod.content) std::visit(check_caller, c);

	check_version_is_single(mod.meta_params, make_path(mod.name));
	check_deprication_is_single(mod.meta_params, make_path(mod.name));
}

void modegen::checker::check(const modegen::record& r, const std::string& path) const
{
	check_version_is_overmin(r.meta_params, make_path(path, r.name));
	std::vector<std::string> nl;
	for(auto& rm:r.members) nl.emplace_back(rm.name);
	check_names(nl, make_path(path,r.name));
}

void modegen::checker::check(const modegen::function& f, const std::string& path) const
{
	check_version_is_overmin(f.meta_params, make_path(path, f.name));
}

void modegen::checker::check(const modegen::interface& i, const std::string& path) const
{
	check_version_is_overmin(i.meta_params, make_path(path, i.name));

	std::vector<std::string> nl;
	for(auto& fnc:i.mem_funcs) nl.emplace_back(fnc.name);
	check_names(nl, make_path(path,i.name));

	auto old_min_ver = cur_min_ver;
	upgrade_cur_min_ver(i);

	for(auto& fnc:i.mem_funcs) {
		if(!fnc.is_mutable.has_value())
			throw error_info(cur_file, make_path(path,i.name,fnc.name), "all methods must to be const or mutable");
		check(fnc, make_path(path, i.name));
	}

	cur_min_ver = old_min_ver;
}

void modegen::checker::check(modegen::enumeration& e, const std::string& path) const
{
	check_version_is_overmin(e.meta_params, make_path(path, e.name));

	std::vector<std::string> nl;
	for(auto& en:e.elements) nl.emplace_back(en.name);
	check_names(nl, make_path(path,e.name));

	if(e.gen_io) for(auto& en:e.elements) if(en.io.empty()) en.io = en.name;
}

void modegen::checker::check_names(std::vector<std::string> nl, const std::string& path) const
{
	auto check = [&nl](const std::string& v) {for(auto& n:nl)if(n==v)return false; return true;};
	while(nl.size()!=0) {
		auto name = std::move(nl.back());
		nl.pop_back();
		if(!check(name)) throw error_info(cur_file, path, "duplicate name " + name);
	}
}

void modegen::checker::check_version_is_single(const modegen::meta_parameters::parameter_set& p, const std::string& path) const
{
	bool par_was = false;
	for(auto& par:p.set) {
		if(std::holds_alternative<meta_parameters::version>(par)) {
			if(par_was) throw error_info(cur_file, path, "double version parameter");
			else par_was = true;
		}
	}
}

void modegen::checker::check_version_is_overmin(const modegen::meta_parameters::parameter_set& p, const std::string& path) const
{
	for(auto& par:p) {
		if(auto pv=std::get_if<meta_parameters::version>(&par); pv) {
			if(*pv < cur_min_ver) throw error_info(cur_file, path, "veresion in module must be minimum");
		}
	}
}

void modegen::checker::check_deprication_is_single(const modegen::meta_parameters::parameter_set& p, const std::string& path) const
{
	bool dep_was = false;
	for(auto& par:p.set) {
		if(std::holds_alternative<meta_parameters::deprication>(par)) {
			if(dep_was) throw error_info(cur_file, path, "double deprication parameter");
			else dep_was = true;
		}
	}
}

bool modegen::checker::is_same_ver(const modegen::module& m1, const modegen::module& m2) const
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

	throw error_info(cur_file, m1.name + " | " + m2.name, "one of this modules has no version tag");
}

bool modegen::checker::combine(modegen::module& to, modegen::module& from) const
{
	assert(to.name == from.name);

	if(!is_same_ver(to, from)) return false;

	to.file_name += "; " + from.file_name;
	for(auto& cnt:from.content) to.content.emplace_back(std::move(cnt));
	for(auto& imp:from.imports) to.imports.emplace_back(std::move(imp)); //TODO: duplicate imports?

	//std::optional<meta_parameters::deprication> to_dep_param;
	//for(auto& par:to.meta_params) if(std::holds_alternative<meta_parameters::deprication>(par)) to_dep_param.emplace(std::move(par));

	return true;
}
