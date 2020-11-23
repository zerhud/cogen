/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "cpp_compiler.hpp"
#include "sep_nodes.hpp"

using namespace std::literals;
using ix3::utils::details::cpp_compiler;

cpp_compiler::cpp_compiler(const gen_utils::compilation_config* c) : config(c) {}

void cpp_compiler::aspect(const ix3_node_base& node, boost::json::object& res) const {}
void cpp_compiler::aspect(const module_node& node, boost::json::object& res) const {}

void cpp_compiler::aspect(const module_version_node& node, boost::json::object& res) const
{
	res["name"] =
			node.val.name + "_v" +
			std::to_string(node.val.version.major_v) + '_' +
			std::to_string(node.val.version.minor_v);
}

void cpp_compiler::aspect(const function_node& node, boost::json::object& res) const {}
void cpp_compiler::aspect(const record_node& node, boost::json::object& res) const {}