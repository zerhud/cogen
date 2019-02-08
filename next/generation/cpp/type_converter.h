#pragma once

#include <map>
#include "parser/modegen.hpp"

namespace modegen {
namespace helpers {

class type_converter final {
public:
	explicit type_converter();
	std::vector<modegen::module>& operator() (std::vector<modegen::module>& mods) ;
	std::vector<std::string> includes() const ;
private:
	struct type_info {
		modegen::type type;
		modegen::module_content_selector points=modegen::module_content_selector::all;
	};

	void convert(modegen::function& obj) ;
	void convert(modegen::record& obj) ;
	void convert(modegen::enumeration& obj) ;
	void convert(modegen::interface& obj) ;

	void convert(modegen::type& t, const modegen::export_info& ei) ;

	void define_type(std::string_view name, modegen::module_content_selector from) ;

	void add_to_imports(std::string mn, bool sys, std::optional<export_info> ei);

	module_content_selector cur_sel;
	modegen::module* cur_mod=nullptr;
	std::vector<std::string> total_incs;
	std::vector<type_info> defined_types;

	static std::map<std::string,std::string> type_maps;
	static std::map<std::string,std::string> incs_maps;
};

} // namespace helpers
} // namespace modegen
