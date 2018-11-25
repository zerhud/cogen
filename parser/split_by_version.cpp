#include "split_by_version.hpp"

#include <cassert>

void modegen::split_by_version::operator ()(std::vector<modegen::module>& mods)
{
	for(auto mod = mods.rbegin(); mod!=mods.rend(); ++mod) {
		cur_name = mod->name;
		set_base(mods);
		analize_mod(mods);
		mods.resize(mods.size()-1);
	}
}

std::optional<modegen::meta_parameters::version> modegen::split_by_version::get_ver(const modegen::meta_parameters::parameter_set& set) const
{
	using modegen::meta_parameters::version;
	for(auto& par:set) if(std::holds_alternative<version>(par)) return std::get<version>(par);
	return std::nullopt;
}

void modegen::split_by_version::analize_mod(std::vector<modegen::module>& mods)
{
}

void modegen::split_by_version::set_base(const std::vector<modegen::module>& mods)
{
	std::optional<modegen::meta_parameters::version> min_ver;
	for(auto& mod:mods) {
		if(mod.name != cur_name) continue;
		auto ver_param = get_ver(mod.meta_params);
		assert(ver_param);
		if( !min_ver || *ver_param < *min_ver) {
			*min_ver = *ver_param;
			set_base(mod);
		}
	}
}

void modegen::split_by_version::set_base(const modegen::module& mod)
{
	//TODO: copy it more effective
	base_mod = mod;
	base_mod.content.clear();
}
