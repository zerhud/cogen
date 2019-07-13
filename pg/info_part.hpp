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

namespace modegen::pg {

/// a single part in info file. manages lang compiler.
class info_part : public part_descriptor {
	enum class fgmode { single, bymod, byent };
	provider_const_ptr prov_;
	options::part_view setts;
	std::vector<output_descriptor_ptr> outs_;
	std::vector<part_algos_ptr> algos_;

	void create_algos(const provider& prov) ;
	std::tuple<fgmode,std::string> outinfo() const ;
public:
	info_part(options::part_view s);

	output_lang lang() const override ;
	std::string_view name() const override ;
	std::vector<output_descriptor_ptr> outputs() const override ;
	void build_outputs(const part_manager& pman, provider_const_ptr prov) override ;
	std::vector<part_algos_ptr> input_managers() const override ;
	std::vector<std::string> map_to_outputs(const std::string& tmpl) const override ;
};

} // namespace modegen::pg::parts

