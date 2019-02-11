#pragma once

#include "parser/interface/modegen.hpp"
#include "common.hpp"

namespace modegen::generation::interface {

class filter final {
	typedef parser::interface::module_content_selector mcselector;
public:
	struct request{
		std::string mod_name;
		std::string cnt_name;
		mcselector sel = mcselector::all;
	};

	explicit filter(const request& q);
	std::vector<parser::interface::module>& operator()(std::vector<parser::interface::module>& mods) const ;
private:
	void filter_by_name(std::vector<parser::interface::module>& mods) const ;
	void remove_without_content(std::vector<parser::interface::module>& mods) const ;
	void cast_options(std::vector<parser::interface::module>& mods) const ;

	const request& query;
};

} // namespace modegen::cvt

