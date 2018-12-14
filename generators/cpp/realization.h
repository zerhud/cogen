#pragma once

#include "../generator.hpp"

namespace modegen {
namespace generators {
namespace cpp {

class realization : public modegen::generator
{
public:
	void set_option(const std::string& key, const std::string& val) override ;
	void generate(mod_selection query, std::vector<modegen::module> mods) const override ;
private:
	static std::string pythongen_path() ;
	static std::string tmpl_path(std::string_view tn) ;

	void gen_hpp(mod_selection query, std::vector<modegen::module> mods) const ;
	void gen_cpp(mod_selection query, std::vector<modegen::module> mods) const ;
	void gen_def(mod_selection query, std::vector<modegen::module> mods) const ;

	void set_out(std::filesystem::path base, std::string_view parsed_file, std::string_view part) const ;

	std::string solve_option(std::string_view name) const ;
	bool solve_bool_option(std::string_view name) const ;

	std::map<std::string,std::string,std::less<>> options;
	mutable std::filesystem::path out_path;
};

} // namespace cpp
} // namespace generators
} // namespace modegen
