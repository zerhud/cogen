/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "tree.hpp"

namespace gen_utils {

class generic_sdl ;
class generic_sdl_factory {
public:
	typedef std::pmr::vector<std::unique_ptr<generic_sdl>> generic_sdl_container;
	typedef std::function<std::filesystem::path(std::filesystem::path)> path_solver;

	virtual ~generic_sdl_factory() noexcept =default ;
	virtual generic_sdl_container languages(path_solver slv) const =0;
};

class generic_sdl {
public:
	virtual ~generic_sdl() noexcept =default ;
	virtual bool empty() const =0 ;
	virtual std::string_view name() const =0 ;
	virtual void add(std::filesystem::path file) =0;
	virtual void finish_loads() =0 ;
	virtual tree data() const =0 ;
};

} // namespace gen_utils
