/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <variant>
#include "traverser.hpp"

using namespace std::literals;

const std::string ix3::utils::traverser::pdel = "."s;

void ix3::utils::traverser::trav_module(const ast::module& mod, trav_direction direction)
{
	parents_.clear();

	auto trav_fnc = overloaded {
		[this](ast::function& o){
			inner_trav(o, [this,&o](){
				for(auto&p:o.params) on_obj(p);
			});
		},
		[this](ast::enumeration& o){
			inner_trav(o, [this,&o](){
				for(auto&e:o.elements) on_obj(e);
			});
		},
		[this](ast::record& o){
			inner_trav(o, [this,&o](){
				for(auto&i:o.members) { on_obj(i); }
			});
		},
		[this](ast::interface& o){
			inner_trav(o, [this,&o](){
				for(auto& m:o.mem_funcs)    { on_obj(m); }
				for(auto& c:o.constructors) { on_obj(c); }
			});
		},
	};

	cur_mod_ = mod;
	cur_direction = direction;

	inner_trav(cur_mod_, [this, &trav_fnc](){
		for(auto& c:cur_mod_.content)
			boost::apply_visitor(trav_fnc, c.var);
		assert(parents_.size()==1);
	});

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
