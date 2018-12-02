#pragma once

#include <map>
#include "modegen.hpp"

namespace modegen {
namespace helpers {

class type_converter final {
public:
	type_converter(module_content_selector s, std::vector<modegen::module>& mods);
private:
	void convert(modegen::function& obj) const ;
	void convert(modegen::record& obj) const ;
	void convert(modegen::enumeration& obj) const ;
	void convert(modegen::interface& obj) const ;

	void convert(modegen::type& t) const ;

	mutable bool do_check = true;
	module_content_selector selector;

	static std::map<std::string,std::string> type_maps;
};

} // namespace helpers
} // namespace modegen
