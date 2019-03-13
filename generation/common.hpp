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

class part_descriptor {
public:
	virtual ~part_descriptor() noexcept =default ;
	virtual std::string origin_name() const =0 ;
	virtual std::string name() const =0 ;
	virtual bool next() const =0 ;
};

class single_part_descriptor : public part_descriptor {
	std::string name_;
public:
	single_part_descriptor(std::string pname);
	~single_part_descriptor() noexcept override ;
	std::string origin_name() const override ;
	std::string name() const override ;
	bool next() const override ;
};

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
	FS::path output_path(std::string_view part) const ;
	FS::path tmpl_path(std::string_view part) const ;
	nlohmann::json generate_data(const part_descriptor& part, const file_data& fdg) const ;
	void build_extra_env(tmpl_gen_env& env, std::string_view part) const ;
	std::string cur_filegen(std::string_view part) const ;
	std::unique_ptr<part_descriptor> part_info(std::string_view p, const file_data& fdg) const ;

	provider_ptr prov;
	options::container_ptr opts;
	FS::path info_directory;
};

} // namespace generation
} //  namespace modegen
