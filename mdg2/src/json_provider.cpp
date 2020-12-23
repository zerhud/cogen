/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "json_provider.hpp"

using mdg2::json_provider;

void json_provider::output_dir(const std::filesystem::path& dir)
{
	outdir = dir;
}

void json_provider::generate(
	        std::filesystem::path tmpl_file,
	        const boost::json::value& data,
	        std::string_view out_file) const
{
	boost::json::object result;
	result["file"] = tmpl_file.string();
	result["data"] = data;
	result["out_file"] = outdir.empty() ? out_file : (outdir / out_file).string();

	all_data.emplace_back(result);
}

boost::json::value json_provider::result() const
{
	decltype(all_data) ret;
	ret.swap(all_data);
	return ret;
}

