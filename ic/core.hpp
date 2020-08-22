/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>
#include <vector>
#include <nlohman/json.hpp>

namespace modegen::ic {

class input {
public:
	virtual ~input() noexcept =default;
	virtual nlohmann::json to_json() const =0 ;
};

class output {
public:
	virtual ~output() noexcept =default;
	virtual void add(std::string name, std::string tmpl, nlohmann::json data) =0 ;
};

class provider {
public:
	virtual ~provider() noexcept =default;
};

class transformation {
public:
	virtual ~transformation() noexcept =default;
	virtual void operator ()(input& data) const =0 ;
};

class configuration {
public:
	struct output_info {
		std::string name;
		std::string tmpl;
	};

	virtual ~configuration() noexcept =default;
	virtual std::vector<output_info> outputs() const =0 ;
	virtual std::vector<std::shared_ptr<transformation>> mutators() const =0 ;
};

class core {
public:
	core() ;
	void gen(std::shared_ptr<input> data, std::shared_ptr<output> out, std::shared_ptr<configuration> config) const ;
};

} // namespace modegen::ic
