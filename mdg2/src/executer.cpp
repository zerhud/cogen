/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "executer.hpp"
#include "searcher.hpp"

#include <cassert>
#include <iostream>

using namespace mdg2;
namespace fs = std::filesystem;
namespace po = boost::program_options;

//boost::program_options::options_description executer::desc("mdg2 options");

executer::executer(int argc, char** argv)
	: desc("mdg2 options")
{
	assert(0 < argc);
	set_pathes(argv[0]);
	set_options();

	auto opts = po::command_line_parser(argc, argv).options(desc).run();
	po::store(opts, opt_vars);
}

void executer::set_pathes(fs::path cur_file)
{
	program_name = cur_file.stem();
	self_path = fs::absolute(cur_file.parent_path()).lexically_normal();
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

std::filesystem::path executer::etc_dir() const
{
#ifdef DEBUG
	return self_path / "etc" / program_name;
#else
	return (self_path / ".." / "etc" / program_name).lexically_normal();
#endif
}

std::filesystem::path executer::tmpl_dir() const
{
	return etc_dir() / "tmpls";
}

std::filesystem::path executer::xmpl_dir() const
{
	return etc_dir() / "examples";
}

int executer::operator()() const
{
	if(opt_vars.count("help"))
		print_help();
	else if(opt_vars["gmode"].as<std::string>()=="json")
		json_mode();
	else if(opt_vars["gmode"].as<std::string>()=="dir")
		dir_mode();
	else {
		std::cerr
			<< "wront generation mode "
			<< opt_vars["gmode"].as<std::string>()
			<< std::endl;
		return 1;
	}

	return 0;
}

void executer::dir_mode() const
{
	std::cerr << "dir mode are not ready yet" << std::endl;
}

void executer::json_mode() const
{
	std::cerr << "json mode are not ready yet" << std::endl;
}

void executer::print_help() const
{
	std::cout
		<< "this is a source code generator. use with options"
		<< std::endl
		<< desc
		<< std::endl;
}
