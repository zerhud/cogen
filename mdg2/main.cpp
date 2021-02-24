/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "src/executer.hpp"
#include "single_gen_part.hpp"
//#include "cppjinja/evtree/evtree.hpp"
//#include "cppjinja/parser/parse.hpp"

int main(int argc,char** argv)
{
	assert(0 < argc);
	mdg2::path_config pcfg(argv[0]);
	mdg2::executer exe(std::move(pcfg), argc, argv);
	//cppjinja::evtree ev;
	return exe();
}
