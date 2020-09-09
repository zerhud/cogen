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
#include "interface/ix3_loader.hpp"
#include "errors.h"

using namespace std::literals;

namespace mp = modegen::parser;

std::vector<std::string> mp::loaders_manager::name_list()
{
	return {"interface"s, "ix3"s, "info"s, "json"s};
}

mp::loaders_manager::loaders_manager()
{
	loaders["idl"] = std::make_shared<interface::ix3_loader>();
	loaders["idl_depricated"] = std::make_shared<interface::loader_impl>();
	loaders["extra"] = std::make_shared<data_tree::loader_impl>(data_tree::loader_impl::data_format::info);
}

mp::loader_ptr mp::loaders_manager::require(std::string_view name)
{
	if(name=="ix3"sv) return loaders["idl"];
	if(name=="interface"sv) return loaders["idl"];
	if(name=="info"sv) {
		auto ret = loaders["extra"];
		static_cast<data_tree::loader_impl*>(ret.get())->next_input_format(data_tree::loader_impl::data_format::info);
		return ret;
	}
	if(name=="json"sv) {
		auto ret = loaders["extra"];
		static_cast<data_tree::loader_impl*>(ret.get())->next_input_format(data_tree::loader_impl::data_format::json);
		return ret;
	}

	throw errors::error("no loader with name "s + std::string(name));
}

std::vector<mp::loader_ptr> mp::loaders_manager::finish_loads()
{
	std::vector<loader_ptr> ret;
	for(auto& l:loaders) ret.emplace_back(l.second)->finish_loads();
	return ret;
}
