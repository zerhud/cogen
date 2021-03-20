/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
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
				for(auto& c:o.constructors) {
					inner_trav(c, [this,&c] {
						for(auto&p:c.params) on_obj(p);
					});
				}
				for(auto& m:o.mem_funcs)    {
					inner_trav(m, [this,&m] {
						for(auto&p:m.params) on_obj(p);
					});
				}
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
