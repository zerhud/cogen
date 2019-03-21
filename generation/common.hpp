/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>
#include <string_view>
#include <nlohman/json.hpp>
#include <boost/property_tree/ptree.hpp>

#include "declaration.hpp"
#include "parser/interface/modegen.hpp"
#include "options.hpp"

namespace modegen {
namespace generation {

/// generates files from info file and parameters
class generator {
public:
	/// @param p provider @param i path to info file
	generator(provider_ptr p, const FS::path& i);

	/// override options in info file
	boost::property_tree::ptree& options() ;
	/// read overriden options fron info file
	/// (added as opposite for mutable options)
	const boost::property_tree::ptree& options() const ;

	/// generate files to output dir
	void generate(const FS::path& output_dir) const ;
	/// generate only one part into std::cout
	void generate_stdout(std::string_view part) const ;
private:
	FS::path tmpl_path(part_descriptor& part) const ;
	void build_extra_env(tmpl_gen_env& env, const part_descriptor& part) const ;
	std::string cur_filegen(const part_descriptor& part) const ;
	std::unique_ptr<part_descriptor> part_info(std::string_view p) const ;

	provider_ptr prov;
	options::container_ptr opts;
	FS::path info_directory;
};

} // namespace generation
} //  namespace modegen
