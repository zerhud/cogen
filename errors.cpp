#include "errors.h"

using namespace std::literals;

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

modegen::errors::gen_error::gen_error(std::string gen_name, std::string w)
    : error(std::move(gen_name), std::move(w))
{
}

modegen::errors::no_data::no_data(std::string_view gen_name) : gen_error(std::string(gen_name), "no data provided"s) {}

modegen::errors::error_info::error_info(std::string f, std::string p, std::string w)
    : modegen::errors::error(f + " " + p + ": ", w)
    , file(std::move(f))
    , path(std::move(p))
    , what(std::move(w))
{
}
