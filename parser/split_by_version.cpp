#include "split_by_version.hpp"

#include <cassert>
#include <algorithm>

using modegen::meta_parameters::version;

void modegen::split_by_version::operator ()(std::vector<modegen::module>& mods)
{
	result.clear();
	for(auto mod = mods.rbegin(); mod!=mods.rend(); ++mod) {
		split_mod(*mod);
	}

	mods = result;
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
	auto cur_ver = modegen::get<version>(obj);
	if(!cur_ver) base_mod().content.emplace_back(obj);
	else add_by_version(obj, *cur_ver);
}

void modegen::split_by_version::split(const modegen::enumeration& obj)
{
	auto cur_ver = modegen::get<version>(obj);
	if(!cur_ver) base_mod().content.emplace_back(obj);
	else add_by_version(obj, *cur_ver);
}

void modegen::split_by_version::split(const modegen::record& obj)
{
	auto bver = modegen::get<version>(obj);

	std::vector<version> obj_vers;
	if(bver) obj_vers.emplace_back(*bver);
	else obj_vers.emplace_back(base_ver());
	for(auto& e:obj.members) if(auto v=modegen::get<version>(e);v) obj_vers.emplace_back(*v);
	std::sort(obj_vers.begin(), obj_vers.end());
	obj_vers.erase(std::unique(obj_vers.begin(), obj_vers.end()), obj_vers.end());

	for(auto& iv:obj_vers) {
		modegen::record base(obj);
		base.members.clear();
		copy_if<version>(obj.members, base.members, [&iv](const std::optional<version>& v){return !v || *v<=iv;});
		add_by_version(base, iv);
	}
}

void modegen::split_by_version::split(const modegen::interface& obj)
{
	auto bver = modegen::get<version>(obj);

	std::vector<version> obj_vers;
	if(bver) obj_vers.emplace_back(*bver);
	else obj_vers.emplace_back(base_ver());
	for(auto& e:obj.mem_funcs) if(auto v=modegen::get<version>(e);v) obj_vers.emplace_back(*v);
	for(auto& e:obj.constructors) if(auto v=modegen::get<version>(e);v) obj_vers.emplace_back(*v);
	std::sort(obj_vers.begin(), obj_vers.end());
	obj_vers.erase(std::unique(obj_vers.begin(), obj_vers.end()), obj_vers.end());

	for(auto& iv:obj_vers) {
		modegen::interface base(obj);
		base.mem_funcs.clear();
		base.constructors.clear();

		auto ch = [&iv](const std::optional<version>& v){ return !v || *v<=iv;};
		copy_if<version>(obj.mem_funcs, base.mem_funcs, ch);
		copy_if<version>(obj.constructors, base.constructors, ch);

		add_by_version(base, iv);
	}
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
	auto ret = modegen::get<version>(base_mod());
	assert(ret);
	return *ret;
}
