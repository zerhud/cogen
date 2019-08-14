/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "parser/interface/modegen.hpp"
#include "common.hpp"

namespace modegen::pg::palgos::interface {

class filter final {
	typedef parser::interface::module_content_selector mcselector;
public:
	struct request{
		std::string mod_name;
		std::string cnt_name;
		mcselector sel = mcselector::all;
	};

	explicit filter(const request& q);
	std::vector<parser::interface::module>& operator()(std::vector<parser::interface::module>& mods) const ;
private:
	void filter_by_name(std::vector<parser::interface::module>& mods) const ;
	void remove_without_content(std::vector<parser::interface::module>& mods) const ;

	const request& query;
};

} // namespace modegen::pg::palgos::interface

