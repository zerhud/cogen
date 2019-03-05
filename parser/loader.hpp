/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <map>
#include "config.hpp"
#include FILESYSTEM

namespace modegen::parser {

class loader;
typedef std::shared_ptr<loader> loader_ptr;

/// allow to create and use loaders
class loaders_manager {
public:
	static std::vector<std::string> name_list() ;

	loaders_manager();

	loader_ptr require(std::string_view name);
	std::vector<loader_ptr> finish_loads() ;
private:
	std::map<std::string, loader_ptr> loaders;
};

class loader {
public:
	virtual ~loader() noexcept =default ;

	virtual void load(std::istream& input, std::string fn) =0 ;
	virtual void load(FS::path file) =0 ;
	virtual void finish_loads() =0 ;
};

} // namespace modegen::parser
