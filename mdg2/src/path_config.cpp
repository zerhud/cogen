/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "path_config.hpp"

using mdg2::path_config;
namespace fs = std::filesystem;

path_config::path_config(const std::filesystem::path& exe_path)
{
	program_name = exe_path.stem();
	self_path = fs::absolute(exe_path.parent_path()).lexically_normal();
	data_pathes.add(".").add(xmpl_dir());
}

std::filesystem::path path_config::xmpl_dir() const
{
	return etc_dir() / "examples";
}

std::filesystem::path path_config::tmpl_dir() const
{
	return etc_dir() / "tmpls";
}

std::filesystem::path path_config::etc_dir() const
{
#ifdef DEBUG
	return self_path / "etc" / program_name;
#else
	return (self_path / ".." / "etc" / program_name).lexically_normal();
#endif
}

std::filesystem::path path_config::input_data(const std::filesystem::path& fn) const
{
	return data_pathes(fn);
}

