/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <map>
#include "parser/interface/modegen.hpp"

namespace modegen::pg::cpp {

class type_converter final {
public:
	explicit type_converter();
	std::vector<parser::interface::module>& operator() (std::vector<parser::interface::module>& mods) ;
	std::vector<std::string> includes() const ;
private:
	struct type_info {
		parser::interface::type type;
		parser::interface::module_content_selector points=parser::interface::module_content_selector::all;
	};

	void convert(parser::interface::function& obj) ;
	void convert(parser::interface::record& obj) ;
	void convert(parser::interface::enumeration& obj) ;
	void convert(parser::interface::interface& obj) ;

	void convert(parser::interface::type& t, const parser::interface::export_info& ei) ;

	void define_type(std::string_view name, parser::interface::module_content_selector from) ;

	void add_to_imports(std::string mn, bool sys, std::optional<parser::interface::export_info> ei);

	parser::interface::module_content_selector cur_sel;
	parser::interface::module* cur_mod=nullptr;
	std::vector<std::string> total_incs;
	std::vector<type_info> defined_types;

	static std::map<std::string,std::string> type_maps;
	static std::map<std::string,std::string> incs_maps;
};

} // namespace modegen::pg::cpp
