/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "split_by_ver.hpp"

using gen_utils::split_by_ver;

std::pmr::vector<gen_utils::tree> split_by_ver::operator()(tree src)
{
	std::pmr::vector<tree> ret;
	while(src.root_version() < src.next_min_version()) {
		auto copied = copy_for_next(src);
		if(copied) ret.emplace_back(*copied);
		src.root_version(src.next_min_version());
	}
	ret.emplace_back(std::move(src));
	return ret;
}

std::optional<gen_utils::tree> split_by_ver::copy_for_next(const tree& src) const
{
	auto rv = src.root_version();
	return src.copy_if([this, rv](const data_node& n){
			return !n.version() || *n.version() <= rv ; });
}
