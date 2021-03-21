/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "path_config.hpp"

using cogen::path_config;
namespace fs = std::filesystem;

path_config::path_config(const std::filesystem::path& exe_path)
{
	program_name = exe_path.stem();
	self_path = fs::absolute(exe_path.parent_path()).lexically_normal();
	data_pathes.add(".").add(etc_dir() / "examples");
	gens_pathes.add(".").add(etc_dir() / "generators");
	libs_pathes.add(".").add(etc_dir() / "lib");
}

std::filesystem::path path_config::etc_dir() const
{
#ifdef DEBUG
	return self_path / "etc" / program_name;
#else
	return (self_path / ".." / "etc" / program_name).lexically_normal();
#endif
}

void path_config::add_input_data(std::filesystem::path p)
{
	data_pathes.add(p);
}

std::filesystem::path path_config::input_data(const std::filesystem::path& fn) const
{
	return data_pathes(fn);
}

std::filesystem::path path_config::generator(const std::filesystem::path& fn) const
{
	return gens_pathes(fn);
}

std::filesystem::path path_config::library(const std::filesystem::path& fn) const
{
	return libs_pathes(fn);
}
