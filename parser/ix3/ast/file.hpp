/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <filesystem>

#include "common.hpp"
#include "entity.hpp"

namespace ix3::ast {

using module_content = x3::variant
	< record
	, function
	, interface
	, enumeration
	>;

struct include_st {
	string_t path;
};

struct module {
	string_t name;
	std::vector<module_content> content;
	meta::version version;
	meta::set meta_params;
};

struct file_content {
	std::vector<module> modules;
	std::vector<include_st> includes;
	std::filesystem::path path;
};

} // namespace ix3::ast

