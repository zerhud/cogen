#pragma once

#include <map>
#include "parser/modegen.hpp"
#include "../case_converter.h"

namespace modegen {
namespace helpers {

class type_converter final {
public:
	explicit type_converter(std::vector<modegen::module>& mods);
	std::vector<std::string> includes() const ;
private:
	struct type_info {
		modegen::type type;
		modegen::module_content_selector points=modegen::module_content_selector::all;
	};

	void convert(modegen::function& obj) const ;
	void convert(modegen::record& obj) const ;
	void convert(modegen::enumeration& obj) const ;
	void convert(modegen::interface& obj) const ;

	void convert(modegen::type& t) const ;

	void solve_type(std::string_view name, modegen::module_content_selector from) const ;

	modegen::module* cur_mod=nullptr;
	std::vector<std::string> total_incs;
	mutable std::vector<type_info> defined_types;

	static std::map<std::string,std::string> type_maps;
	static std::map<std::string,std::string> incs_maps;
};

} // namespace helpers
} // namespace modegen
