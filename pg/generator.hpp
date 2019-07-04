/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "declarations.hpp"
#include "part_manager.hpp"

namespace modegen::pg {

class generator final {
	provider_ptr prov;
	options::container_ptr setts;
	part_manager pman;

	void init_parts();
	void build_deps();
public:
	generator(provider_ptr p, options::container_ptr s);

	void build_env() ;
	part_manager& parts() ;

	void generate(const FS::path& output_dir) const ;
	void generate(std::string_view part, std::ostream& out) const ;
};

} // namespace modegen::pg
