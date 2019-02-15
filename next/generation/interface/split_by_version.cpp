#include "split_by_version.hpp"

#include <cassert>
#include <algorithm>

using modegen::parser::interface::meta_parameters::version;
namespace mi = modegen::parser::interface;

modegen::generation::interface::split_by_version::split_by_version(bool dry_run)
	: dry(dry_run)
{
}

std::vector<mi::module>& modegen::generation::interface::split_by_version::operator ()(std::vector<mi::module>& mods)
{
	if(dry) return mods;

	result.clear();
	for(auto mod = mods.rbegin(); mod!=mods.rend(); ++mod) {
		split_mod(*mod);
	}

	mods.swap(result);

	return mods;
}

void modegen::generation::interface::split_by_version::split_mod(const mi::module& mod)
{
	set_base(mod);
	auto splitter = [this](const auto& cnt) { split(cnt); };
	for(auto& c:mod.content) std::visit(splitter, c);
}

void modegen::generation::interface::split_by_version::set_base(const mi::module& mod)
{
	TODO(copy it more effective)
	//mi::module base_mod = mod;
	//base_mod.content.clear();
	result.emplace_back(mod).content.clear();
	base_index = result.size() - 1;
}

void modegen::generation::interface::split_by_version::split(const mi::function& obj)
{
	auto cur_ver = mi::get<version>(obj);
	if(!cur_ver) base_mod().content.emplace_back(obj);
	else add_by_version(obj, *cur_ver);
}

void modegen::generation::interface::split_by_version::split(const mi::enumeration& obj)
{
	auto cur_ver = mi::get<version>(obj);
	if(!cur_ver) base_mod().content.emplace_back(obj);
	else add_by_version(obj, *cur_ver);
}

void modegen::generation::interface::split_by_version::split(const mi::record& obj)
{
	auto bver = mi::get<version>(obj);

	std::vector<version> obj_vers;
	if(bver) obj_vers.emplace_back(*bver);
	else obj_vers.emplace_back(base_ver());
	for(auto& e:obj.members) if(auto v=mi::get<version>(e);v) obj_vers.emplace_back(*v);
	std::sort(obj_vers.begin(), obj_vers.end());
	obj_vers.erase(std::unique(obj_vers.begin(), obj_vers.end()), obj_vers.end());

	for(auto& iv:obj_vers) {
		mi::record base(obj);
		base.members.clear();
		copy_if<version>(obj.members, base.members, [&iv](const std::optional<version>& v){return !v || *v<=iv;});
		add_by_version(base, iv);
	}
}

void modegen::generation::interface::split_by_version::split(const mi::interface& obj)
{
	auto bver = mi::get<version>(obj);

	std::vector<version> obj_vers;
	if(bver) obj_vers.emplace_back(*bver);
	else obj_vers.emplace_back(base_ver());
	for(auto& e:obj.mem_funcs) if(auto v=mi::get<version>(e);v) obj_vers.emplace_back(*v);
	for(auto& e:obj.constructors) if(auto v=mi::get<version>(e);v) obj_vers.emplace_back(*v);
	std::sort(obj_vers.begin(), obj_vers.end());
	obj_vers.erase(std::unique(obj_vers.begin(), obj_vers.end()), obj_vers.end());

	for(auto& iv:obj_vers) {
		mi::interface base(obj);
		base.mem_funcs.clear();
		base.constructors.clear();

		auto ch = [&iv](const std::optional<version>& v){ return !v || *v<=iv;};
		copy_if<version>(obj.mem_funcs, base.mem_funcs, ch);
		copy_if<version>(obj.constructors, base.constructors, ch);

		add_by_version(base, iv);
	}
}

mi::module& modegen::generation::interface::split_by_version::base_mod()
{
	assert(0<result.size());
	assert(base_index<result.size());
	return result[base_index];
}

const mi::module& modegen::generation::interface::split_by_version::base_mod() const
{
	assert(0<result.size());
	assert(base_index<result.size());
	return result[base_index];
}

mi::meta_parameters::version modegen::generation::interface::split_by_version::base_ver() const
{
	auto ret = mi::get<version>(base_mod());
	assert(ret);
	return *ret;
}
