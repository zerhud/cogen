/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <boost/program_options.hpp>

#include "path_config.hpp"
#include "json_provider.hpp"
#include "parser/ix3/parser.hpp"
#include "ic/single_gen_part.hpp"
#include "ic/ptsetts.hpp"

namespace cogen {

class executer {
	boost::program_options::options_description desc;
	boost::program_options::variables_map opt_vars;
	path_config pathes;

	gen_utils::input user_data;

	void set_options();
	bool can_continue() const ;

	void print_help() const ;
	void dir_mode() const ;
	void json_mode(const mdg::ic::ptsetts& setts) const ;
	void load_inputs() ;
	void load_inludes() ;
	boost::property_tree::ptree load_settings() const ;
public:
	executer(path_config pc, int argc, char** argv);
	int operator()() ;
};

} // namespace mdg2

