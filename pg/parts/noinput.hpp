/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <regex>
#include <vector>
#include "pg/part_descriptor.hpp"
#include "pg/options.hpp"

namespace modegen::pg::parts {

/// a single part in info file. manages lang compiler.
class nointput : public part_descriptor {
	provider_ptr prov;
	options::part_view setts;
	std::vector<output_descriptor_ptr> outs_;
public:
	module_part(provider_ptr p, options::part_view s);

	output_lang lang() const override ;
	std::string_view name() const override ;
	std::vector<output_descriptor_ptr> outputs() const override ;
	void build_outputs(const part_manager& pman, const provider& prov) override ;
};

} // namespace modegen::pg::parts

