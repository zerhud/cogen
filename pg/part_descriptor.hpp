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
#include "declarations.hpp"

namespace modegen::pg {

class part_descriptor {
public:
	virtual ~part_descriptor() noexcept =default ;

	virtual output_lang lang() const =0 ;
	virtual std::string_view name() const =0 ;
	virtual std::vector<output_descriptor_ptr> outputs(const std::vector<std::regex>& filter={}) const =0 ;
	virtual void build_outputs(const part_manager& pman, const provider& prov) =0 ;
};

class module_part : public part_descriptor {
	std::string name_;
	output_lang lang_;
	std::vector<output_descriptor_ptr> outs_;
public:
	module_part(output_lang l, std::string n);

	output_lang lang() const override ;
	std::string_view name() const override ;
	std::vector<output_descriptor_ptr> outputs(const std::vector<std::regex>& filter={}) const override ;
	void build_outputs(const part_manager& pman, const provider& prov) override ;
};

} // namespace modegen::pg

