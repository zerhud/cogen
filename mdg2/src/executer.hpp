/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <filesystem>
#include <boost/program_options.hpp>

#include "searcher.hpp"
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

	std::filesystem::path self_path;
	std::filesystem::path program_name;

	searcher data_pathes;

	ix3::parser ix3_loader;
	modegen::ic::input user_data;
	std::shared_ptr<json_provider> json_out;

	void set_pathes(std::filesystem::path cur_file);
	void set_options();

	std::filesystem::path etc_dir() const ;
	std::filesystem::path tmpl_dir() const ;
	std::filesystem::path xmpl_dir() const ;
	void print_help() const ;
	void dir_mode() const ;
	void json_mode() const ;
	void load_inputs() ;
public:
	executer(int argc, char** argv);
	int operator()() ;
};

} // namespace mdg2

