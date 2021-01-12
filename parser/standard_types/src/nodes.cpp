/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "nodes.hpp"

using std_types::root_node;
using std_types::type_node;

std::string_view root_node::name() const
{
	return "";
}

std::pmr::vector<gen_utils::name_t> root_node::required_links() const
{
	return {};
}

std::string_view root_node::link_condition() const
{
	return "";
}

std::optional<gen_utils::import_file> root_node::imports_modification() const
{
	return std::nullopt;
}

std::optional<std::uint64_t> root_node::version() const
{
	return 0;
}

std::optional<gen_utils::variable> root_node::node_var() const
{
	return std::nullopt;
}

boost::json::value root_node::to_json(const gen_utils::tree& con) const
{
	boost::json::array cnt;
	for(auto& c:con.children(*this))
		cnt.emplace_back(static_cast<const base_node*>(
					c.get())->to_json(con));
	return cnt;
}


type_node::type_node(
        std::string name,
        std::string tgt,
        std::string rpl,
        std::pmr::string f,
        bool sys)
    : type(std::move(name))
    , target(std::move(tgt))
    , replacer(std::move(rpl))
    , to_file(std::move(f))
    , is_sys(sys)
{
}

std::string_view type_node::name() const
{
	return type;
}

std::pmr::vector<gen_utils::name_t> type_node::required_links() const
{
	return {};
}

std::string_view type_node::link_condition() const
{
	return target;
}

std::optional<gen_utils::import_file> type_node::imports_modification() const
{
	return gen_utils::import_file{is_sys, to_file};
}

std::optional<std::uint64_t> type_node::version() const
{
	return 0;
}

std::optional<gen_utils::variable> type_node::node_var() const
{
	return std::nullopt;
}

boost::json::value type_node::to_json(const gen_utils::tree&) const
{
	boost::json::value ret = replacer.c_str();
	return ret;
}
