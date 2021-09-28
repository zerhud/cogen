/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ic/provider.hpp"
#include "path_config.hpp"

namespace cogen {

class flist_provider : public cogen::ic::provider {
	mutable std::pmr::vector<std::pmr::string> flist;
	const path_config& pathes;
public:
	flist_provider(const path_config& pcfg);
	void generate(
	        std::filesystem::path tmpl_file,
	        const boost::json::value& data,
	        std::string_view out_file) const override ;
	std::pmr::vector<std::pmr::string> result() const ;
};

} // namespace cogen


