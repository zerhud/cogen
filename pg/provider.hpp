/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include "declarations.hpp"
#include "parser/loader.hpp"

namespace modegen::pg {

class provider {
public:
	virtual ~provider() noexcept =default ;
	virtual part_descriptor_ptr create_part(options::part_view&& ps) const =0 ;
	virtual output_descriptor_ptr create_output(output_lang lng, std::filesystem::path p) const =0 ;
	virtual std::vector<parser::loader_ptr> input() const =0 ;
	//virtual FS::path resolve_file(const FS::path& p, const FS::path& assumed, std::string_view gen_name) const =0 ;
	//virtual void json_jinja(const tmpl_gen_env& data) const =0 ;
};

} // namespace modegen::pg

