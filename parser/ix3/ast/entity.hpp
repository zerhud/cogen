/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "meta.hpp"
#include "common.hpp"
#include "functions.hpp"

namespace ix3::ast {

struct enum_element {
	single_variable_name name;
	std::string io;
};
struct enumeration {
	single_variable_name name;
	std::vector<enum_element> elements;
	meta::set meta_params;

	bool gen_io=false;
	bool use_bitmask=false;
};


struct record_item {
	single_variable_name name;
	type param_type;
	meta::set meta_params;
	bool is_required = true;
};

struct record {
	single_variable_name name;
	std::vector<record_item> members;
	meta::set meta_params;
	bool use_as_exception = false;
};


struct interface {
	single_variable_name name;
	std::vector<function> mem_funcs;
	std::vector<constructor> constructors;
	meta::set meta_params;
	bool realization_in_client = false;
	bool use_as_exception = false;
};


} // namespace ix3::ast
