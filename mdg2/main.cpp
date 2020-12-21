/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "src/executer.hpp"
#include "generation_part.hpp"
//#include "cppjinja/evtree/evtree.hpp"
//#include "cppjinja/parser/parse.hpp"

int main(int argc,char** argv)
{
	mdg2::executer exe(argc, argv);
	return exe();
	//cppjinja::evtree ev;
	//return 0;
}
