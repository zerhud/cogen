#pragma once

#include "parser/modegen.hpp"

namespace modegen {

template<typename Cvt>
std::result_of_t<Cvt(std::vector<modegen::module>&)> operator | (std::vector<modegen::module>& m, Cvt& c)
{
	return c(m);
}

template<typename Cvt>
std::result_of_t<Cvt(std::vector<modegen::module>&)> operator | (std::vector<modegen::module>& m, const Cvt& c)
{
	return c(m);
}

template<typename Cvt>
std::result_of_t<Cvt(std::vector<modegen::module>&)> operator | (std::vector<modegen::module>& m, Cvt&& c)
{
	return c(m);
}

} // namespace modegen

