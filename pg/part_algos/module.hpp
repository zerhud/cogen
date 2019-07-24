/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../part_algos.hpp"
#include "parser/interface/loader.hpp"

namespace modegen::pg::palgos {

//TODO: rename to module or modules
class module_algos : public part_algos {
	std::vector<parser::interface::module> mods_;
	std::shared_ptr<modegen::parser::interface::loader> mldr_;
	bool replace(std::string& tmpl, const std::string& var_name, const std::string& value) const ;
public:
	module_algos(const std::vector<modegen::parser::loader_ptr>& ldrs);
	void set_filter(const options::part_view& pinfo) override ;
	std::vector<std::string> map(const std::string& tmpl) const override ;
	std::vector<modegen::parser::interface::module> mods() const ;
};

} // namespace modegen::pg::palgos


