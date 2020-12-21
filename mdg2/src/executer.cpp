/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "executer.hpp"

#include <cassert>
#include <iostream>

using namespace mdg2;
namespace fs = std::filesystem;

executer::executer(int argc, char** argv)
{
	assert(0 < argc);
	set_pathes(argv[0]);
}

void executer::set_pathes(fs::path cur_file)
{
	program_name = cur_file.stem();
	self_path = fs::absolute(cur_file.parent_path()).lexically_normal();
}

std::filesystem::path executer::etc_dir() const
{
#ifdef DEBUG
	return self_path / "etc" / program_name;
#else
	return (self_path / ".." / "etc" / program_name).lexically_normal();
#endif
}

std::filesystem::path executer::tmpl_dir() const
{
	return etc_dir() / "tmpls";
}

int executer::operator()() const
{
	std::cout << tmpl_dir() << std::endl;
	return 0;
}
