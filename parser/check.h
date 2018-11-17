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
	void check_func(const function& f) const ;
	void check_methd(const function& f) const ;
	void check_class(const interface& i) const ;
	void check_enum(const enumeration& e) const ;

	void check_names(std::vector<std::string> nl, const std::string& path) const ;

	std::string cur_file;
};

} // namespace modegen
