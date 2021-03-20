/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "common.hpp"

namespace ix3::ast {

class meta_parameter {
public:
	virtual ~meta_parameter() noexcept =default ;
	virtual std::string_view name() const =0;
	virtual std::string value() const =0;
};

namespace meta {
	struct version;
	bool operator == (const version& left, const version& right) noexcept ;

	struct version : meta_parameter, x3::position_tagged {
		std::int32_t major_v;
		std::int32_t minor_v;
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

	struct oapi : meta_parameter, x3::position_tagged {
		std::string key;
		std::string val;
		std::string_view name() const override ;
		std::string value() const override ;
	};

	struct set : x3::position_tagged {
		typedef variant_t<version, depricated, documentation, oapi> parameter_t;
		set& operator = (std::vector<parameter_t> v) noexcept ;

		std::vector<parameter_t> cnt;
	};
} // namespace meta
} // namespace ix3::ast
