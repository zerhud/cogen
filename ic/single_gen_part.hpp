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
#include "core.hpp"

namespace mdg::ic {

struct gen_config;
struct gen_context;
typedef std::pmr::map<std::pmr::string, gen_utils::input> compiled_output;

class single_gen_part final {
	const provider* outside;
	compiled_output compile(const gen_context& setts,
	                        const gen_utils::input& data) const ;
	boost::json::value make_json(
	        const gen_context& setts, const gen_utils::input& data) const ;
	std::pmr::vector<std::pmr::string> matched_includes(
	        const gen_context& setts,
	        const std::pmr::string& link,
	        const gen_utils::input& data) const ;
public:
	single_gen_part(const provider* p);
	compiled_output operator()(const gen_context& cur_part,
	                           gen_utils::input alli) const ;
};

struct gen_config {
	std::pmr::string map_tmpl;
	std::pmr::string tmpl_file;
	std::pmr::vector<std::pmr::string> links;
	gen_utils::compilation_config compilation;
};

struct gen_context {
	gen_config cfg_part;
	std::pmr::map<std::pmr::string, compiled_output> generated;
};

} // namespace modegen::ic
