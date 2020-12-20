/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once
#include <boost/property_tree/ptree.hpp>
#include "core.hpp"

namespace modegen::ic {

struct gen_settings {
	std::string_view map_tmpl;
	std::string_view tmpl_file;
	gen_utils::compilation_config* gen_cfg;
};

class single_gen_part final {
	std::shared_ptr<provider> outside;
public:
	single_gen_part(std::shared_ptr<provider> p);
	void operator()(gen_settings cur_part, input alli) const ;
};

} // namespace modegen::ic
