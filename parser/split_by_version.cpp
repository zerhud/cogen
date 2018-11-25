#include "split_by_version.hpp"

#include <cassert>

void modegen::split_by_version::operator ()(std::vector<modegen::module>& mods)
{
	result.clear();
	for(auto mod = mods.rbegin(); mod!=mods.rend(); ++mod) {
		split_mod(*mod);
	}

	mods = result;
}

void modegen::split_by_version::set_ver(modegen::meta_parameters::parameter_set& set, const modegen::meta_parameters::version& v) const
{
	using modegen::meta_parameters::version;
	for(auto& par:set) {
		if(std::holds_alternative<version>(par)) {
			std::get<version>(par) = v;
			return;
		}
	}

	set.set.emplace_back(v);
}

std::optional<modegen::meta_parameters::version> modegen::split_by_version::get_ver(const modegen::meta_parameters::parameter_set& set) const
{
	using modegen::meta_parameters::version;
	for(auto& par:set) if(std::holds_alternative<version>(par)) return std::get<version>(par);
	return std::nullopt;
}

void modegen::split_by_version::split_mod(const modegen::module& mod)
{
	set_base(mod);
	auto splitter = [this](const auto& cnt) { split(cnt); };
	for(auto& c:mod.content) std::visit(splitter, c);
}

void modegen::split_by_version::set_base(const modegen::module& mod)
{
	//TODO: copy it more effective
	//modegen::module base_mod = mod;
	//base_mod.content.clear();
	result.emplace_back(mod).content.clear();
	base_index = result.size() - 1;
}

void modegen::split_by_version::split(const modegen::function& obj)
{
	auto cur_ver = get_ver(obj.meta_params);
	if(!cur_ver) base_mod().content.emplace_back(obj);
	else add_by_version(obj, *cur_ver);
}

void modegen::split_by_version::split(const modegen::enumeration& obj)
{
	auto cur_ver = get_ver(obj.meta_params);
	if(!cur_ver) base_mod().content.emplace_back(obj);
	else add_by_version(obj, *cur_ver);
}

void modegen::split_by_version::split(const modegen::record& obj)
{
	//modegen::enumeration base(obj);
	//base.elements.clear();

	//std::size_t total_copied=0;
	//std::optional<modegen::meta_parameters::version> last_ver;
	//while(total_copied < obj.elements.size()) {
	    //for(auto& el:obj.elements) {
	        //auto el_ver = get_ver(el.)
	    //}
	//}
}

void modegen::split_by_version::split(const modegen::interface& obj)
{

}

modegen::module& modegen::split_by_version::base_mod()
{
	assert(0<result.size());
	assert(base_index<result.size());
	return result[base_index];
}

const modegen::module& modegen::split_by_version::base_mod() const
{
	assert(0<result.size());
	assert(base_index<result.size());
	return result[base_index];
}

modegen::meta_parameters::version modegen::split_by_version::base_ver() const
{
	auto ret = get_ver(base_mod().meta_params);
	assert(ret);
	return *ret;
}
