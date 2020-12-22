/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "executer.hpp"

#include <regex>
#include <cassert>
#include <iostream>

#include "ix3/utils/to_generic_ast.hpp"

using namespace mdg2;
using mdg2::path_config;
using namespace std::literals;
namespace fs = std::filesystem;
namespace po = boost::program_options;

executer::executer(path_config pc, int argc, char** argv)
	: desc("mdg2 options")
	, json_out(std::make_shared<json_provider>())
	, pathes(std::move(pc))
{
	assert(0 < argc);
	set_options();

	auto opts = po::command_line_parser(argc, argv).options(desc).run();
	po::store(opts, opt_vars);
}

void executer::set_options()
{
	desc.add_options()
		("help,h", "produce this help message")
		("outdir,o", po::value<std::string>(), "directory or file where to output")
		("gmode,m", po::value<std::string>()->default_value("json"), "generation mode (\"json\" for generate json and \"dir\" to generate files)")
		("input,i", po::value<std::vector<std::string>>(), "input (foramt like -Iinterface=some_file). use - for read from std input")
		;
}

int executer::operator()()
{
	if(opt_vars.count("help"))
		print_help();
	load_inputs();
	if(opt_vars["gmode"].as<std::string>()=="json")
		json_mode();
	else if(opt_vars["gmode"].as<std::string>()=="dir")
		dir_mode();
	else {
		std::cerr
			<< "wrong generation mode "
			<< std::quoted(opt_vars["gmode"].as<std::string>())
			<< std::endl;
		return 1;
	}

	return 0;
}

void executer::load_inputs()
{
	ix3::parser ix3_loader;
	std::regex key_val_parser("([0-9a-zA-Z_.]+)(=(.+))?", std::regex::egrep);
	for(auto& input:opt_vars["input"].as<std::vector<std::string>>()) {
		std::cmatch m;
		std::regex_match(input.data(), m, key_val_parser);
		if(m[1] == "ix3") ix3_loader.parse(pathes.input_data(m[3].str()));
		else throw std::runtime_error("parser "s + m[1].str() + " are not found"s);
	}
	ix3_loader.finish_loads();
	user_data.add( ix3::utils::to_generic_ast()(ix3_loader.result()) );
}

void executer::dir_mode() const
{
	std::cerr << "dir mode are not ready yet" << std::endl;
}

void executer::json_mode() const
{
	modegen::ic::single_gen_part part(json_out);
	modegen::ic::gen_settings setts;
	default_config compil_config;
	setts.gen_cfg = &compil_config;
	part(setts, user_data);
	std::cout << json_out->result() << std::endl;
}

void executer::print_help() const
{
	std::cout
		<< "this is a source code generator. use with options"
		<< std::endl
		<< desc
		<< std::endl;
}
