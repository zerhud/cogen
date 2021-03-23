/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <iostream>

namespace ix3::text {

class parser_env_tag;
class parser_env
{
public:
	virtual ~parser_env() noexcept =default ;
	virtual std::string file_name() const =0 ;
	virtual void on_error() const =0 ;
	virtual void on_error(const std::string& message) const =0 ;
	virtual std::ostream& out() const =0 ;
	virtual std::string msg(const std::string& whcih) const =0 ;
};

class cerr_throw_env : public parser_env {
	std::string fn;
public:
	cerr_throw_env(std::string fn="");
	std::string file_name() const override ;
	void on_error() const override ;
	void on_error(const std::string& message) const override ;
	std::ostream& out() const override ;
	std::string msg(const std::string& which) const override ;
};

} // namespace ix3::text
