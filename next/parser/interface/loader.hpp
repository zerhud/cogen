#pragma once

#include <filesystem>
#include "check.hpp"
#include "modegen.hpp"
#include "parser/loader.hpp"

namespace modegen::parser::interface {

class loader : public parser::loader {
public:
	loader();
	loader(std::vector<std::filesystem::path> includes);

	void load(std::istream& input, std::string fn) override ;
	void load(std::filesystem::path file) override ;

	std::vector<modegen::parser::interface::module> result() ;
private:
	bool already_loaded(const std::filesystem::path f) const ;
	std::filesystem::path search_file(std::filesystem::path f) const ;

	std::vector<std::filesystem::path> incs;
	std::vector<std::filesystem::path> cur_dir;
	std::vector<std::filesystem::path> loaded_files;

	checker ch;
};

} // namespace modegen::parser::interface

