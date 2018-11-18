#include "check.h"

const std::string modegen::checker::pdel = ".";

modegen::error_info::error_info(std::string f, std::string p, std::string w)
    : std::runtime_error("in " + f + " " + p + ": " + w)
    , file(std::move(f))
    , path(std::move(p))
    , what(std::move(w))
{
}

void modegen::checker::operator ()(std::vector<modegen::module>& mods, std::string_view file_name) const
{
	for(auto& mod:mods) {
		mod.file_name = file_name;
		check_mod(mod);
	}
}

void modegen::checker::check_mod(const modegen::module& mod) const
{
	std::vector<std::string> nl;
	auto name_collector = [&nl](const auto& e){nl.emplace_back(e.name);};
	for(auto& c:mod.content) std::visit(name_collector, c);
	check_names(std::move(nl), mod.name);

	auto check_caller = [this,&mod](const auto& v){check(v, mod.name);};
	for(auto& c:mod.content) std::visit(check_caller, c);
}

void modegen::checker::check(const modegen::record& r, const std::string& path) const
{
	std::vector<std::string> nl;
	for(auto& rm:r.members) nl.emplace_back(rm.name);
	check_names(nl, make_path(path,r.name));
}

void modegen::checker::check(const modegen::function& f, const std::string& path) const
{

}

void modegen::checker::check(const modegen::interface& i, const std::string& path) const
{
	std::vector<std::string> nl;
	for(auto& fnc:i.mem_funcs) nl.emplace_back(fnc.name);
	check_names(nl, make_path(path,i.name));

	for(auto& fnc:i.mem_funcs)
		if(!fnc.is_mutable.has_value())
			throw error_info(cur_file, make_path(path,i.name,fnc.name), "all methods must to be const or mutable");
}

void modegen::checker::check(const modegen::enumeration& e, const std::string& path) const
{
	check_names(e.elements, make_path(path,e.name));
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
