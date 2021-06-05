/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "src/executer.hpp"
#include "single_gen_part.hpp"

int main(int argc,char** argv)
{
	assert(0 < argc);
	cogen::program_configuration conf{ argv[0], "@PROJECT_VERSION@" };
	cogen::executer exe(std::move(conf), argc, argv);
	return exe();
}
