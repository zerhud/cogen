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
#include "ic/generation_part.hpp"

namespace mdg2 {

struct default_config : gen_utils::compilation_config {
	gen_utils::compiler compiler_name () const { return gen_utils::compiler::cpp; }
	std::string_view value(std::string_view key) const { return ""; }
	gen_utils::name_conversion naming() const { return gen_utils::name_conversion::as_is; }
};

class executer {
	boost::program_options::options_description desc;
	boost::program_options::variables_map opt_vars;

	path_config pathes;

	ix3::parser ix3_loader;
	modegen::ic::input user_data;
	std::shared_ptr<json_provider> json_out;

	void set_options();

	void print_help() const ;
	void dir_mode() const ;
	void json_mode() const ;
	void load_inputs() ;
public:
	executer(path_config pc, int argc, char** argv);
	int operator()() ;
};

} // namespace mdg2

