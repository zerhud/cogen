#include "errors.h"

modegen::errors::error::error(std::string w) noexcept
    : what_(std::move(w))
{
}

modegen::errors::error::error(std::string where, std::string w)
    : what_(std::move(where))
{
	what_ += ": " + std::move(w);
}

const char *modegen::errors::error::what() const noexcept
{
	return what_.c_str();
}
