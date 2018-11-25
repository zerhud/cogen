#pragma once

#include <map>
#include "modegen.hpp"

namespace modegen {

class split_by_version {
public:
	void operator () (std::vector<modegen::module>& mods) ;
private:
	std::optional<modegen::meta_parameters::version> get_ver(const modegen::meta_parameters::parameter_set& set) const ;
	void analize_mod(std::vector<modegen::module>& mods);
	void set_base(const std::vector<modegen::module>& mods);
	void set_base(const modegen::module& mod);

	std::map<std::uint64_t, modegen::module> cur_mods;
	std::string cur_name;
	std::vector<modegen::module> result;
	modegen::module base_mod;
};

} // namespace modegen
