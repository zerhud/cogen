/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once
#include <map>
#include <boost/property_tree/ptree.hpp>

#include "provider.hpp"
#include "utils/input.hpp"

namespace mdg::ic {

struct gen_config;
struct gen_context;
typedef std::pmr::map<std::pmr::string, gen_utils::input> compiled_output;

class single_gen_part final {
	const provider* outside;
	gen_utils::input split_by_vers(
	        const gen_config& setts, gen_utils::input data) const ;
	const gen_utils::tree& select(
	        const gen_config& setts, const gen_utils::input& data) const ;
	boost::json::object to_json(const gen_utils::import_file& f) const ;
	compiled_output compile(const gen_context& setts,
	                        const gen_utils::input& data) const ;
	boost::json::value make_json(
	        const gen_context& setts,
	        const gen_utils::input& data,
	        const gen_utils::imports_manager& imports) const ;
	gen_utils::imports_manager make_imports(
	        const gen_context& setts,
	        const compiled_output& result) const ;
	void add_includes_to_result(
	        boost::json::object& result,
	        gen_utils::imports_manager::incs_map_t incs
	        ) const ;
	void add_includes_to_result(
	        boost::json::object& result,
	        const gen_utils::input& data,
	        const gen_utils::imports_manager& imports) const ;
public:
	single_gen_part(const provider* p);
	compiled_output operator()(const gen_context& cur_part,
	                           gen_utils::input alli) const ;
};

struct gen_config {
	std::pmr::string map_tmpl;
	std::pmr::string tmpl_file;
	std::pmr::vector<std::pmr::string> links;
	bool split_by_version = false;
	gen_utils::compilation_config compilation;
	std::pmr::string map_from = "ix3";
};

struct gen_context {
	gen_config cfg_part;
	std::pmr::map<std::pmr::string, compiled_output> generated;
};

} // namespace modegen::ic
