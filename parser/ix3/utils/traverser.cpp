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
	mstack_.clear();


	auto before = [this](auto& o){
		make_path(cur_mod_.name, o.name);
		mstack_.push_back(o.meta_params);
	};

	auto trav_fnc = overloaded {
		[this](ast::function& o)   { on_obj(o); },
		[this](ast::enumeration& o){ on_obj(o); },
		[this,&before](ast::record& o){
			for(auto&i:o.members) { on_obj(i); }
			on_obj(o);
		},
		[this,&before](ast::interface& o){
			for(auto& m:o.mem_funcs)    { on_obj(m); }
			for(auto& c:o.constructors) { on_obj(c); }
			on_obj(o);
		},
	};

	cur_mod_ = mod;
	mstack_.push_back(cur_mod_.meta_params);

	for(auto& c:cur_mod_.content) {
		boost::apply_visitor(before, c.var);
		boost::apply_visitor(trav_fnc, c.var);
		mstack_.pop_back();
		on_obj(cur_mod_);
	}

	mstack_.pop_back();
}

ix3::ast::module& ix3::utils::traverser::module()
{
	return cur_mod_;
}

std::string ix3::utils::traverser::path() const
{
	return path_;
}

std::vector<ix3::ast::meta::set> ix3::utils::traverser::meta_stack() const
{
	return mstack_;
}

