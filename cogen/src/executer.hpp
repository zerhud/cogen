/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
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

struct program_configuration {
	path_config pathes;
	std::pmr::string version;
};

class executer {
	boost::program_options::options_description desc;
	boost::program_options::variables_map opt_vars;
	program_configuration config;

	gen_utils::input user_data;

	void set_options();
	bool can_continue() const ;

	std::unique_ptr<std::ostream> create_out_file(std::string fn) const ;
	void print_help() const ;
	void dir_mode(const ic::ptsetts& setts) const ;
	void json_mode(const cogen::ic::ptsetts& setts) const ;
	void load_inputs() ;
	void load_inludes() ;
	boost::property_tree::ptree load_settings() const ;
	json_provider create_json(const ic::ptsetts& setts) const ;
public:
	executer(program_configuration pc, int argc, char** argv);
	int operator()() ;
};

} // namespace cogen

