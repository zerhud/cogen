#pragma once

#include <map>
#include "modegen.hpp"

namespace modegen {

class split_by_version {
public:
	void operator () (std::vector<modegen::module>& mods) ;
private:
	std::optional<modegen::meta_parameters::version> get_ver(const modegen::meta_parameters::parameter_set& set) const ;
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

	template<typename O>
	void add_by_version(const O& o, const modegen::meta_parameters::version& v)
	{
		for(std::size_t i=base_index;i<result.size();++i) {
			auto cur_ver = *get_ver(result[i].meta_params);
			if(cur_ver <= v) result[i].content.emplace_back(o);
			if(cur_ver == v) return;
		}

		result.emplace_back(base_mod()).content.emplace_back(o);
	}
};

} // namespace modegen
