#pragma once

#include <vector>
#include <string>
#include "parser/modegen.hpp"

namespace modegen::cvt {

enum class name_conversion{underscore, camel_case, title_case, as_is};
std::string_view to_string(name_conversion c);
name_conversion from_string(std::string_view n);

class naming {
public:
	naming(name_conversion c);
	void operator () (std::vector<module>& mods) const ;

	static std::vector<std::string> split_name(const std::string& name);
	static std::string convert(const std::string& name, name_conversion c);
private:
	name_conversion conver;
};

} // namespace modegen::cvt

