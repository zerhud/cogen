/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <variant>
#include "traverser.hpp"

using namespace std::literals;

const std::string ix3::utils::traverser::pdel = "."s;

void ix3::utils::traverser::trav_module(const ast::module& mod)
{
	parents_.clear();

	auto before = [this](auto& o){
		make_path(cur_mod_.name, o.name);
	};

	auto trav_fnc = overloaded {
		[this](ast::function& o)   { on_obj(o); },
		[this](ast::enumeration& o){ on_obj(o); },
		[this,&before](ast::record& o){
			before(o);
			parents_.emplace_back(&o);
			for(auto&i:o.members) { on_obj(i); }
			parents_.pop_back();
			on_obj(o);
		},
		[this,&before](ast::interface& o){
			before(o);
			parents_.emplace_back(&o);
			for(auto& m:o.mem_funcs)    { on_obj(m); }
			for(auto& c:o.constructors) { on_obj(c); }
			parents_.pop_back();
			on_obj(o);
		},
	};

	cur_mod_ = mod;

	parents_.emplace_back(&cur_mod_);
	for(auto& c:cur_mod_.content) {
		boost::apply_visitor(before, c.var);
		boost::apply_visitor(trav_fnc, c.var);
	}

	assert(parents_.size()==1);
	on_obj(cur_mod_);
	parents_.clear();
}

ix3::ast::module& ix3::utils::traverser::module()
{
	return cur_mod_;
}

std::string ix3::utils::traverser::path() const
{
	return path_;
}
