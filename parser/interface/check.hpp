/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "grammar.hpp"

namespace modegen::parser::interface {

class checker {
public:
	checker& operator ()(modegen::parser::interface::parsed_file& finfo) ;
	std::vector<module> extract_result() ;
private:
	void unite_mods(std::vector<module>& mods) const ;

	void check_mod(module& mod) const ;
	void check(const record& f, const std::string& path) const ;
	void check(const function& f, const std::string& path) const ;
	void check(const interface& i, const std::string& path) const ;
	void check(enumeration& e, const std::string& path) const ;

	void check_using(const modegen::parser::interface::module& mod) const ;
	void check_names(const modegen::parser::interface::module& mod) const ;
	void check_names(std::vector<std::string> nl, const std::string& path) const ;
	void check_version_is_single(const meta_parameters::parameter_set& p, const std::string& path) const ;
	void check_version_is_overmin(const meta_parameters::parameter_set& p, const std::string& path) const ;
	void check_deprication_is_single(const meta_parameters::parameter_set& p, const std::string& path) const ;
	bool is_same_ver(const module& m1, const module& m2) const ;

	bool combine(module& to, module& from) const ;

	void collect_exports(module& mod) const ;

	template<typename T, typename... Args>
	static std::string make_path(T&& v1, Args... v)
	{
		if constexpr(sizeof...(Args)==0) return v1;
		else return v1 + pdel + make_path(std::forward<Args>(v)...);
	}

	template<typename T>
	void upgrade_cur_min_ver(const T& t) const
	{
		for(auto& p:t.meta_params) {
			if(auto pv=std::get_if<meta_parameters::version>(&p); pv) cur_min_ver = *pv;
		}
	}

	std::vector<module> resulting_set;
	std::string cur_file;
	mutable modegen::parser::interface::meta_parameters::version cur_min_ver;
	const static std::string pdel;
};

} // namespace modegen::parser::interface

