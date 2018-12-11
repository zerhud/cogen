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

} // namespace errors
} // namespace modegen
