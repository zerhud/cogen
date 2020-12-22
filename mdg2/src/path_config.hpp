/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <filesystem>

#include "searcher.hpp"

namespace mdg2 {

class path_config final {
	std::filesystem::path self_path;
	std::filesystem::path program_name;

	searcher data_pathes;
	searcher gens_pathes;

	std::filesystem::path etc_dir() const ;
	std::filesystem::path xmpl_dir() const ;
	std::filesystem::path tmpl_dir() const ;
	std::filesystem::path gens_dir() const ;
public:
	path_config(const std::filesystem::path& exe_path);

	std::filesystem::path input_data(const std::filesystem::path& fn) const ;
	std::filesystem::path generator(const std::filesystem::path& fn) const ;
};

} // namespace mdg2
