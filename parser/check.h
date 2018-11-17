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
	void operator ()(std::vector<module>& mods, std::string_view file_name) const ;
	void operator ()(std::vector<module>& mods) const ;
private:
	void check_mod(const module& mod) const ;
	void check(const record& f, const std::string& path) const ;
	void check(const function& f, const std::string& path) const ;
	void check(const interface& i, const std::string& path) const ;
	void check(const enumeration& e, const std::string& path) const ;

	void check_names(std::vector<std::string> nl, const std::string& path) const ;

	std::string cur_file;
	const static std::string pdel;
};

} // namespace modegen
