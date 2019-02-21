/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "config.hpp"
#include FILESYSTEM
#include <cppjson/json.h>

namespace modegen {
namespace generation {

class json_jinja_generator {
public:
	virtual ~json_jinja_generator() noexcept =default ;
	virtual const json_jinja_generator& operator() (FS::path tmpl, std::optional<FS::path> out, const cppjson::value& data) const =0 ;
};


class jinja_python_generator : public json_jinja_generator {
public:
	jinja_python_generator(FS::path gen);
	const json_jinja_generator& operator () (FS::path tmpl, std::optional<FS::path> out, const cppjson::value& data) const override ;
private:
	FS::path generator;
};

} // namespace generators
} // namespace modegen
