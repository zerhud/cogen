/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <map>
#include <functional>
#include "parser/interface/modegen.hpp"
#include "parser/interface/helpers.hpp"

namespace modegen::pg::palgos::interface {

class split_version {
public:
	split_version(bool dry_run = false);
	std::vector<parser::interface::module>& operator () (std::vector<parser::interface::module>& mods) ;
private:
	void split_mod(parser::interface::module mod);
	void prepare_modules(const parser::interface::module& mod);
	std::vector<parser::interface::meta_parameters::version> all_versions(const parser::interface::module& mod) const ;
	void insert_content(const parser::interface::module_content& cnt);
	void deep_copy(const parser::interface::record& from, parser::interface::record& to);
	void deep_copy(const parser::interface::interface& from, parser::interface::interface& to);

	bool dry = false;
	std::size_t current_mod = 0;
	std::vector<parser::interface::module> result;
};

} // namespace modegen::pg::palgos::interface
