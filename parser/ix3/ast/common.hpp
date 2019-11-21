/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../common.hpp"

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace ix3::ast {

namespace x3 = boost::spirit::x3;

using string_t = std::string;

template<typename T>
using forward_ast = x3::forward_ast<T>;

template<typename... Args>
using variant_t = x3::variant<Args...>;

struct quoted1_string : string_t, x3::position_tagged {};
struct quoted2_string : string_t, x3::position_tagged {};
struct single_variable_name : string_t, x3::position_tagged {};
struct variable_name : std::vector<string_t>, x3::position_tagged {};

struct type : x3::position_tagged {
	variable_name name;
	std::vector<forward_ast<type>> sub_types;
};

class meta_parameter {
public:
	virtual ~meta_parameter() noexcept =default ;
	virtual std::string_view name() const =0;
	virtual std::string value() const =0;
};

namespace meta {
	struct version : meta_parameter, x3::position_tagged {
		std::int64_t major_v;
		std::int64_t minor_v;
		~version() noexcept override =default ;
		std::string_view name() const override ;
		std::string value() const override ;
	};

	struct depricated : meta_parameter, x3::position_tagged {
		std::string message;
		version since;
		~depricated() noexcept override =default ;
		std::string_view name() const override ;
		std::string value() const override ;
	};

	struct documentation : meta_parameter, x3::position_tagged {
		std::string body;
		~documentation() noexcept override =default ;
		std::string_view name() const override ;
		std::string value() const override ;
	};

	struct set {
		typedef variant_t<version, depricated, documentation> parameter_t;
		std::vector<parameter_t> cnt;
	};
} // namespace meta

} // namespace ix3::ast

