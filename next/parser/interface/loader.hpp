#pragma once

#include <filesystem>
#include "check.hpp"
#include "modegen.hpp"
#include "parser/loader.hpp"

namespace modegen::parser::interface {

class loader : public parser::loader {
public:
	virtual std::vector<module> result() const =0 ;
};

class loader_impl : public loader {
public:
	loader_impl();
	loader_impl(std::vector<std::filesystem::path> includes);

	void load(std::istream& input, std::string fn) override ;
	void load(std::filesystem::path file) override ;
	void finish_loads() override ;

	std::vector<module> result() const override ;
private:
	bool already_loaded(const std::filesystem::path f) const ;
	std::filesystem::path search_file(std::filesystem::path f) const ;

	std::vector<std::filesystem::path> incs;
	std::vector<std::filesystem::path> cur_dir;
	std::vector<std::filesystem::path> loaded_files;

	checker ch;
	std::vector<module> result_cache;
};

} // namespace modegen::parser::interface

