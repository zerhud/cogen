#pragma once

#include "parser/modegen.hpp"
#include "generators/generator.hpp"

namespace modegen::cvt {

class filter final {
public:
	explicit filter(const generation_request& q);
	std::vector<modegen::module>& operator()(std::vector<modegen::module>& mods) const ;
private:
	void filter_by_name(std::vector<modegen::module>& mods) const ;
	void remove_without_content(std::vector<modegen::module>& mods) const ;
	void cast_options(std::vector<modegen::module>& mods) const ;

	const generation_request& query;
};

} // namespace modegen::cvt

