/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "check.hpp"
#include "modegen.hpp"
#include "loader.hpp"

namespace modegen::parser::interface {
	
class ix3_loader : public loader {
public:
	ix3_loader();
	ix3_loader(std::vector<FS::path> includes);

	void load(std::istream& input, std::string fn) override ;
	void load(FS::path file) override ;
	void finish_loads() override ;

	std::vector<module> result() const override ;
};

} // namespace modegen::parser::interface

