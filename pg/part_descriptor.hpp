/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <regex>
#include <vector>
#include "declarations.hpp"

namespace modegen::pg {

class part_descriptor {
public:
	virtual ~part_descriptor() noexcept =default ;

	virtual FS::path tmpl_file() const =0 ;
	virtual output_lang lang() const =0 ;
	virtual std::string_view name() const =0 ;
	virtual std::vector<output_descriptor_ptr> outputs() const =0 ;
	virtual void build_outputs(const part_manager& pman, provider_const_ptr prov) =0 ;
	virtual std::vector<part_algos_ptr> input_managers() const =0 ;
	virtual std::vector<std::string> map_to_outputs(const std::string& tmpl) const =0 ;
};

} // namespace modegen::pg
