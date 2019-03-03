/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>
#include <string_view>
#include <nlohman/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include "parser/loader.hpp"
#include "options.hpp"

namespace modegen::generation {

class options_view final {
	const boost::property_tree::ptree& opts;
	std::string_view part;
public:
	options_view(const boost::property_tree::ptree& o, std::string_view p);

	std::string_view part_name() const ;
	boost::property_tree::ptree part_data() const ;
	boost::property_tree::ptree part_data(std::string_view name) const ;
	boost::property_tree::ptree target_data(std::string_view name) const ;
	const boost::property_tree::ptree& all() const ;

	std::optional<std::string> part_str(const std::string& path, std::string_view name="") const ;
	std::optional<std::string> target_str(const std::string& path, std::string_view name) const ;
	std::vector<std::string> part_str_list(const std::string& path, std::string_view key, std::string_view name="") const ;

	std::string naming() const ;
};

class file_data {
public:
	virtual ~file_data() noexcept =default ;
	virtual nlohmann::json jsoned_data(const std::vector<parser::loader_ptr>& data_loaders, options::view opts) const =0 ;
};

} // namespace modegen::generation
