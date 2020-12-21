/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <filesystem>
#include <boost/program_options.hpp>

namespace mdg2 {

class executer {
	boost::program_options::options_description desc;
	boost::program_options::variables_map opt_vars;

	std::filesystem::path self_path;
	std::filesystem::path program_name;

	void set_pathes(std::filesystem::path cur_file);
	void set_options();

	std::filesystem::path etc_dir() const ;
	std::filesystem::path tmpl_dir() const ;
	std::filesystem::path xmpl_dir() const ;
	void print_help() const ;
	void dir_mode() const ;
	void json_mode() const ;
public:
	executer(int argc, char** argv);
	int operator()() const ;
};

} // namespace mdg2

