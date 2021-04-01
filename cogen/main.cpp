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
	cogen::path_config pcfg(argv[0]);
	cogen::executer exe(std::move(pcfg), argc, argv);
	return exe();
}
