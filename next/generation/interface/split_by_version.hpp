#pragma once

#include <map>
#include <functional>
#include "parser/interface/modegen.hpp"
#include "parser/interface/helpers.hpp"

namespace modegen::generation::interface {

TODO(move to converers and make converter from it)
class split_by_version {
	bool dry = false;
public:
	split_by_version(bool dry_run = false);
	std::vector<parser::interface::module>& operator () (std::vector<parser::interface::module>& mods) ;
private:
	void split_mod(const parser::interface::module& mod);
	void set_base(const parser::interface::module& mod);

	void split(const parser::interface::function& obj);
	void split(const parser::interface::enumeration& obj);
	void split(const parser::interface::record& obj);
	void split(const parser::interface::interface& obj);

	parser::interface::module& base_mod() ;
	const parser::interface::module& base_mod() const ;
	parser::interface::meta_parameters::version base_ver() const ;

	std::vector<parser::interface::module> result;
	std::size_t base_index=0;

	template<typename Prop, typename Content>
	void copy_if(const std::vector<Content>& from, std::vector<Content>& to, const std::function<bool(const std::optional<Prop>&)>& pred) const
	{
		for(auto& c:from) if(pred(parser::interface::get<Prop>(c))) to.emplace_back(c);
	}

	template<typename O>
	parser::interface::module& add_by_version(const O& o, const parser::interface::meta_parameters::version& v)
	{
		using parser::interface::meta_parameters::version;
		for(std::size_t i=base_index;i<result.size();++i) {
			auto cur_ver = *parser::interface::get<version>(result[i].meta_params);
			if(v <= cur_ver) result[i].content.emplace_back(o);
			if(cur_ver == v) return result[i];
		}

		parser::interface::module& imod = result.emplace_back(base_mod());
		imod.content.emplace_back(o);
		parser::interface::set<version>(imod.meta_params, v);
		return imod;
	}
};

} // namespace modegen::generation::interface
