#include "check.h"

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
