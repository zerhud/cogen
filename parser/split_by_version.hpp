#pragma once

#include <map>
#include <functional>
#include "modegen.hpp"
#include "helpers.hpp"

namespace modegen::converters {

class split_by_version {
public:
	void operator () (std::vector<modegen::module>& mods) ;
private:
	void split_mod(const modegen::module& mod);
	void set_base(const modegen::module& mod);

	void split(const modegen::function& obj);
	void split(const modegen::enumeration& obj);
	void split(const modegen::record& obj);
	void split(const modegen::interface& obj);

	modegen::module& base_mod() ;
	const modegen::module& base_mod() const ;
	modegen::meta_parameters::version base_ver() const ;

	std::vector<modegen::module> result;
	std::size_t base_index=0;

	template<typename Prop, typename Content>
	void copy_if(const std::vector<Content>& from, std::vector<Content>& to, const std::function<bool(const std::optional<Prop>&)>& pred) const
	{
		for(auto& c:from) if(pred(modegen::get<Prop>(c))) to.emplace_back(c);
	}

	template<typename O>
	modegen::module& add_by_version(const O& o, const modegen::meta_parameters::version& v)
	{
		using modegen::meta_parameters::version;
		for(std::size_t i=base_index;i<result.size();++i) {
			auto cur_ver = *modegen::get<version>(result[i].meta_params);
			if(v <= cur_ver) result[i].content.emplace_back(o);
			if(cur_ver == v) return result[i];
		}

		modegen::module& imod = result.emplace_back(base_mod());
		imod.content.emplace_back(o);
		modegen::set<version>(imod.meta_params, v);
		return imod;
	}
};

} // namespace modegen::converters
