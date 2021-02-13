/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "loader.hpp"

#include "nodes.hpp"
#include "manager.hpp"

using namespace std::literals;

std::optional<gen_utils::tree> builders::loader::operator()(
    boost::property_tree::ptree setts,
    const mdg::ic::gen_context& ctx) const
{
	std::optional<gen_utils::tree> ret;
	auto pch = setts.get_optional<std::string>("project"s);
	if(!pch) return ret;
	ret.emplace(std::make_shared<root_node>(), std::make_shared<dsl_manager>());
	auto proj = std::make_shared<project>(*pch, setts.get("version", "0.0.0.0"s));
	ret->add(ret->root(), proj);
	for(auto& [name, opts]:setts.get_child("libraries"))
		ret->add(*proj, std::make_shared<library>(name, opts, ctx));
	return ret;
}
