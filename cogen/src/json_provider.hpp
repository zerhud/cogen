/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ic/provider.hpp"
#include "path_config.hpp"

namespace cogen {

class json_provider : public cogen::ic::provider {
	mutable boost::json::array all_data;
	std::filesystem::path outdir;
	const path_config& pathes;
public:
	json_provider(const path_config& pcfg);
	void output_dir(const std::filesystem::path& dir);
	void generate(
	        std::filesystem::path tmpl_file,
	        const boost::json::value& data,
	        std::string_view out_file) const override ;
	boost::json::value result() const ;
};

} // namespace cogen

