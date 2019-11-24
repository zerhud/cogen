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
	class details;
	std::unique_ptr<details> pimpl_;
	details& pimpl() { return *pimpl_; }
	const details& pimpl() const { return *pimpl_; }
public:
	ix3_loader();
	ix3_loader(std::vector<FS::path> includes);
	~ix3_loader() noexcept override ;

	void load(std::istream& input, std::string fn) override ;
	void load(FS::path file) override ;
	void finish_loads() override ;

	std::vector<module> result() const override ;
};

} // namespace modegen::parser::interface

