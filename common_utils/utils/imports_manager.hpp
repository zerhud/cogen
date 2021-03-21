/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "declarations.hpp"

#include <map>

namespace gen_utils {

struct import_info {
	node_pointer to;
	node_pointer from;
	import_file file;
	std::pmr::string cond;
	compilation_config cfg;
};

inline const bool operator == (const import_info& l, const import_info& r)
{
	return
	        std::tie(l.to, l.from, l.file, l.cond) ==
	        std::tie(r.to, r.from, l.file, l.cond);
}
inline const bool operator < (const import_info& l, const import_info& r)
{
	return
	        std::tie(l.to, l.from, l.file, l.cond) <
	        std::tie(r.to, r.from, l.file, l.cond);
}

class imports_manager final {
	struct input_info {
		std::pmr::string file;
		std::pmr::string part;
		const input* data;
	};

	std::pmr::vector<input_info> input_store;

	std::pmr::vector<import_info> required_for_scan(
	        const tree& src, const data_node& par) const ;
	std::pmr::vector<import_info> required_for_links(
	        const tree& src, node_ptr cur) const ;
	std::pmr::vector<import_info> remove_own_part(
	        const input& file_data, std::pmr::vector<import_info> src) const ;
	std::pmr::vector<import_info> remove_itself(
	        const input& file_data, std::pmr::vector<import_info> src) const ;
	std::pmr::vector<import_info> required_for(const input& file_data) const ;

	static std::pmr::vector<import_info> unique(
	        std::pmr::vector<import_info> src);
public:
	using  incs_map_t = std::pmr::map<std::pmr::string, std::pmr::vector<import_file>>;

	void build();
	imports_manager& operator()(const std::pmr::string& part,
	                            const std::pmr::string& file,
	                            const input& data);
	imports_manager& add(const std::pmr::string& part,
	                     const std::pmr::string& file,
	                     const input& data);

	std::pmr::vector<import_info> required_for(const tree& file_data) const ;

	incs_map_t required_includes(const input& file_data) const ;
	incs_map_t required_includes_with_own(const input& file_data) const ;
	incs_map_t mapped_includes(std::string_view tmpl, const input& src) const ;
};

} // namespace gen_utils
