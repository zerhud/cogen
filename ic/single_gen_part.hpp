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

namespace modegen::ic {

struct gen_settings;

class single_gen_part final {
	const provider* outside;
	boost::json::value make_json(
			const gen_settings& setts,
			const gen_utils::tree& data) const ;
	boost::json::value make_json(
	        const gen_settings& setts, const input& data) const ;
	std::pmr::map<std::pmr::string, input> compile(
	        const gen_settings& setts, const input& data) const ;
public:
	single_gen_part(const provider* p);
	void operator()(const gen_settings& cur_part, input alli) const ;
};

struct gen_settings {
	std::pmr::string map_tmpl;
	std::pmr::string tmpl_file;
	gen_utils::compilation_config* gen_cfg;
	std::pmr::vector<std::pmr::string> links;
	std::pmr::map<std::pmr::string, single_gen_part> generated;
};

} // namespace modegen::ic
