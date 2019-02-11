#pragma once

#include "file_data.hpp"

namespace modegen::generation {

class cpp_generator : public file_data {
public:
	cppjson::value jsoned_data(parser::loader_ptr data_loader, options_view opts) const override ;
private:
	struct inc_info {
		inc_info(std::string n, bool s) : name(std::move(n)), sys(s) {}
		std::string name;
		bool sys;
	};

	std::vector<inc_info> includes(const std::vector<std::string> sys, options_view& opts) const ;
	std::string solve_part_include(const std::string& part, options_view& opts) const ;
};

} // namespace modegen::generation
