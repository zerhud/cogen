#pragma once

#include <map>
#include "modegen.hpp"

namespace modegen {
namespace helpers {

class type_converter final {
public:
	type_converter(module_content_selector s, std::vector<modegen::module>& mods);
	std::vector<std::string> includes() const ;
private:
	void convert(modegen::function& obj) const ;
	void convert(modegen::record& obj) const ;
	void convert(modegen::enumeration& obj) const ;
	void convert(modegen::interface& obj) const ;

	void convert(modegen::type& t) const ;

	module_content_selector selector;
	modegen::module* cur_mod=nullptr;
	std::vector<std::string> total_incs;

	static std::map<std::string,std::string> type_maps;
	static std::map<std::string,std::string> incs_maps;
};

} // namespace helpers
} // namespace modegen
