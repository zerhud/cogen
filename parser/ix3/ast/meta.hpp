/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
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

	struct oapi : meta_parameter, x3::position_tagged {
		std::string key;
		std::string val;
		std::string_view name() const override ;
		std::string value() const override ;
	};

	struct set {
		typedef variant_t<version, depricated, documentation> parameter_t;
		std::vector<parameter_t> cnt;
	};
} // namespace meta
} // namespace ix3::ast
