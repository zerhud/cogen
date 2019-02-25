/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "loader.hpp"
#include "interface/loader.hpp"
#include "data_tree/loader.hpp"

using namespace std::literals;

modegen::parser::loader_ptr modegen::parser::create_loader(std::string_view name, std::vector<FS::path> includes)
{
	if(name == "interface"sv) return std::make_shared<interface::loader_impl>(std::move(includes));
	if(name == "info"sv) return std::make_shared<data_tree::loader_impl>(data_tree::loader_impl::data_format::info);
	if(name == "json"sv) return std::make_shared<data_tree::loader_impl>(data_tree::loader_impl::data_format::json);
	return nullptr;
}
