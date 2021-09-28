/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "flist_provider.hpp"

using cogen::flist_provider;

flist_provider::flist_provider(const path_config& pcfg)
	: pathes(pcfg)
{
}

void flist_provider::output_dir(const std::filesystem::path& dir)
{
	outdir = dir;
}

void flist_provider::generate(
		std::filesystem::path tmpl_file,
	        const boost::json::value& data,
	        std::string_view out_file) const
{
	flist.emplace_back(out_file);
}

std::pmr::vector<std::pmr::string> flist_provider::result() const
{
	decltype(flist) ret;
	ret.swap(flist);
	return ret;
}

