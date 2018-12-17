#pragma once

#include "grammar.hpp"

namespace modegen {

struct error_info : std::runtime_error {
	std::string file;
	std::string path;
	std::string what;
	error_info(std::string f, std::string p, std::string w) ;
};

class checker {
public:
	checker& operator ()(modegen::parsed_file& finfo) ;
	std::vector<module> extract_result() ;
private:
	void unite_mods(std::vector<module>& mods) const ;

	void check_mod(module& mod) const ;
	void check(const record& f, const std::string& path) const ;
	void check(const function& f, const std::string& path) const ;
	void check(const interface& i, const std::string& path) const ;
	void check(enumeration& e, const std::string& path) const ;

	void check_names(const modegen::module& mod) const ;
	void check_names(std::vector<std::string> nl, const std::string& path) const ;
	void check_version_is_single(const meta_parameters::parameter_set& p, const std::string& path) const ;
	void check_version_is_overmin(const meta_parameters::parameter_set& p, const std::string& path) const ;
	void check_deprication_is_single(const meta_parameters::parameter_set& p, const std::string& path) const ;
	bool is_same_ver(const module& m1, const module& m2) const ;

	bool combine(module& to, module& from) const ;

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
	mutable modegen::meta_parameters::version cur_min_ver;
	const static std::string pdel;
};

} // namespace modegen
