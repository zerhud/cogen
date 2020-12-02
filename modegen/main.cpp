/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <regex>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <boost/program_options.hpp>
#include <boost/json.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/process.hpp>
#include "ic/core.hpp"
#include "ix3/parser.hpp"
#include "ix3/utils/to_generic_ast.hpp"

namespace po = boost::program_options;

using namespace std::literals;

class gen_prov : public modegen::ic::provider
{
public:
	void generate(
		  std::filesystem::path tmpl_file,
		  const boost::json::value& data,
		  std::string_view out_file) const override
	{

	};
};
/*
auto parse_command_line(int argc, char** argv, std::vector<std::string> fglist, std::vector<std::string> gen_list)
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce this help message")
		("input,I", po::value<std::vector<std::string>>(), "input (foramt like -Iinterface=some_file). use - for read from std input")
		("generator,g", po::value<std::vector<std::string>>(), "info file for generation (file contains options)")
		("option,O", po::value<std::vector<std::string>>(), "override option from info file")
		("aoption", po::value<std::vector<std::string>>(), "add option to info file parser result")
		("include", po::value<std::vector<std::string>>(), "include directories for search files")
		("outdir", po::value<std::vector<std::string>>(), "directory where to output (will not be overrdien with new generator)")
		;

	auto opts = po::command_line_parser(argc,argv).options(desc).run();
	po::variables_map vm;
	po::store(opts, vm);

	if(vm.count("help")) {
		std::cout << "use this program to prepare module data." << std::endl << desc << std::endl
		    << std::endl << "as parser you may use:" << std::endl;
		for(auto& g:fglist) std::cout << "\t" << g << std::endl;

		std::cout << std::endl << "as generator you may use:" << std::endl;
		for(auto& g:gen_list) std::cout << "\t" << g << std::endl;

		std::exit(1);
	}

	return std::make_tuple(opts, vm);
}
*/
int main(int argc, char** argv)
{
//	auto prov = std::make_shared<gen_prov>();

//	auto [opts,vm] = parse_command_line(argc, argv, {""}, {""});

	std::string input(argv[1]) ;
	std::string output(argv[2]) ;

	ix3::parser ps ;
	ix3::utils::to_generic_ast tga ;
	ix3::utils::ix3_manager manger ;
	std::filesystem::path path(input) ;
	ps.parse(path) ;
	ps.finish_loads() ;
	auto ast = ps.result() ;
	auto tree = tga(ast) ;

	return 0;
}

