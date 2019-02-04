#pragma once

#include <string>
#include <exception>

namespace modegen {
namespace errors {

struct error : std::exception {
	error(std::string w) noexcept ;
	error(std::string where, std::string w);
	const char* what() const noexcept override ;
	std::string what_;
};

struct gen_error : error {
	gen_error(std::string gen_name, std::string w);
};

struct error_info : error {
	std::string file;
	std::string path;
	std::string what;
	error_info(std::string f, std::string p, std::string w) ;
};

} // namespace errors
} // namespace modegen
