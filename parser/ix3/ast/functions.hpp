/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "meta.hpp"
#include "common.hpp"

namespace ix3::ast {

struct function_parameter {
	single_variable_name name;
	type param_type;
	bool required;
};

struct function {
	single_variable_name name;
	type return_type;
	std::optional<bool> is_mutable;
	std::optional<bool> is_static;
	std::vector<function_parameter> params;
	meta::set meta_params;
};

struct constructor {
	std::vector<function_parameter> params;
	meta::set meta_params;
};

} // namespace ix3::ast

