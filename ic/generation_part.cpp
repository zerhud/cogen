/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "generation_part.hpp"

using modegen::ic::single_gen_part;

single_gen_part::single_gen_part(std::shared_ptr<provider> p)
	: outside(std::move(p))
{
}

void single_gen_part::operator()(gen_settings cur_part, input alli) const
{
	assert(outside);
	boost::json::value data;
	outside->generate(cur_part.tmpl_file, data, cur_part.map_tmpl);
}

