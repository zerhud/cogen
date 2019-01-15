#pragma once

#include <map>
#include "parser/modegen.hpp"
#include "../case_converter.h"

namespace modegen {
namespace helpers {

class type_converter final {
public:
	explicit type_converter(std::vector<modegen::module>& mods, module_content_selector sel=module_content_selector::all);
	std::vector<std::string> includes() const ;
	std::vector<std::string> selected_includes() const ;
private:
	struct type_info {
		modegen::type type;
		modegen::module_content_selector points=modegen::module_content_selector::all;
	};

	void convert(modegen::function& obj) ;
	void convert(modegen::record& obj) ;
	void convert(modegen::enumeration& obj) ;
	void convert(modegen::interface& obj) ;

	void convert(modegen::type& t) ;

	void define_type(std::string_view name, modegen::module_content_selector from) ;

	void add_to_imports(const module_content& obj, using_directive&& d);

	module_content_selector cur_sel;
	modegen::module* cur_mod=nullptr;
	std::vector<std::string> total_incs;
	std::vector<std::string> selected_incs;
	std::vector<type_info> defined_types;

	static std::map<std::string,std::string> type_maps;
	static std::map<std::string,std::string> incs_maps;
};

} // namespace helpers
} // namespace modegen
