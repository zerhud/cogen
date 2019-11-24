/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <variant>
#include <boost/fusion/include/adapt_struct.hpp>


namespace modegen::parser::interface {
class meta_parameter {
public:
	virtual ~meta_parameter() noexcept = default ;
	virtual std::string_view name() const =0;
	virtual std::string value() const =0;
	virtual bool add(const meta_parameter& other) =0 ;
};

namespace meta_parameters {
struct version : meta_parameter {
	version() noexcept ;
	version(std::uint64_t m, std::uint64_t i) noexcept ;

	version(version&&) noexcept =default ;
	version(const version&) noexcept =default ;
	version& operator = (version&& other) noexcept =default ;
	version& operator = (const version& other) noexcept =default ;

	std::uint64_t major_v;
	std::uint64_t minor_v;

	std::string_view name() const override;
	std::string value() const override;
	std::string value(std::string_view sep) const ;
	bool add(const meta_parameter& other) override ;
};

struct documentation : meta_parameter {
	std::string body;

	std::string_view name() const override;
	std::string value() const override;
	bool add(const meta_parameter& other) override ;
};

struct deprication : meta_parameter {
	std::string message;
	version since;

	std::string_view name() const override;
	std::string value() const override;
	bool add(const meta_parameter& other) override ;
};

struct oapi : meta_parameter {
	std::string key;
	std::string val;
	std::string_view name() const override;
	std::string value() const override;
	bool add(const meta_parameter& other) override ;
};

struct parameter_set  {
	typedef std::variant<version,documentation,deprication,oapi> parameter_type;
	std::vector<parameter_type> set;

	void push_back(parameter_type p);
	auto begin() {return set.begin(); }
	auto end() {return set.end(); }
	auto begin() const {return set.begin(); }
	auto end() const {return set.end(); }
};
} // namespace meta_parameters
} // namespace modegen::parser::interface

BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::meta_parameters::version, (std::uint64_t, major_v), (std::uint64_t, minor_v) )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::meta_parameters::documentation, (std::string, body) )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::meta_parameters::deprication, (modegen::parser::interface::meta_parameters::version, since), (std::string, message) )

