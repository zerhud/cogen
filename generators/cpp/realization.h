#pragma once

#include "generator.hpp"

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

	void gen_hpp(mod_selection query, std::vector<modegen::module> mods) const ;
	void gen_cpp(mod_selection query, std::vector<modegen::module> mods) const ;
	void gen_def(mod_selection query, std::vector<modegen::module> mods) const ;

	void set_out(std::filesystem::path base, std::string_view file, std::string_view part) const ;

	std::map<std::string,std::string,std::less<>> options;
	mutable std::filesystem::path out_path;
};

} // namespace cpp
} // namespace generators
} // namespace modegen
