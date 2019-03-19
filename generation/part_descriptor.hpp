/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "options.hpp"
#include "declaration.hpp"
#include "parser/loader.hpp"

namespace modegen {
namespace generation {

class part_descriptor {
public:
	virtual ~part_descriptor() noexcept =default ;
	virtual std::string part_name() const =0 ;
	virtual std::string file_name() const =0 ;
	virtual const options::view& opts() const =0 ;
	virtual bool need_output() const =0 ;
	virtual bool next() =0 ;
};

class single_part_descriptor : public part_descriptor {
	options::view opts_;
public:
	single_part_descriptor(options::view o);
	~single_part_descriptor() noexcept override ;
	std::string part_name() const override ;
	std::string file_name() const override ;
	const options::view& opts() const override ;
	bool need_output() const override ;
	bool next() override ;
};

} // namespace generation
} //  namespace modegen
