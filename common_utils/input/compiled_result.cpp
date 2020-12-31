/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "compiled_result.hpp"

using gen_utils::compiled_result;

bool compiled_result::import_exists(const gen_utils::import_info& src) const
{
	for(auto& i:imports_) if(i.node == src.node) return true;
	return false;
}

void compiled_result::combine(gen_utils::compiled_result other)
{
	for(auto& oi:other.imports_)
		if(!import_exists(oi))
			imports_.emplace_back(std::move(oi));
}

void compiled_result::add_import(gen_utils::import_info i)
{
	imports_.emplace_back(std::move(i));
}

std::pmr::vector<gen_utils::import_info> compiled_result::imports() const
{
	return imports_;
}

boost::json::value compiled_result::data() const
{
	return data_;
}

void compiled_result::data(boost::json::value d)
{
	data_ = d;
}
