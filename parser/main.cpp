#include <fstream>
#include <iostream>
#include <functional>

#include <boost/program_options.hpp>

#include "modegen.hpp"
#include "grammar.hpp"

#include "split_by_version.hpp"
#include "to_json.h"

namespace po = boost::program_options;

std::string read_input(const std::string& file_name)
{
	if(file_name == "-") {
		std::string pdata{std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>()};
		return pdata;
	}

	std::fstream file(file_name);
	std::string pdata{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
	return pdata;
}

void write_output(const cppjson::value& mods, const std::string& file_name)
{
	if(file_name=="-") std::cout << mods << std::endl;
	else {
		std::fstream file(file_name);
		file << mods;
	}
}

int main(int argc,char** argv)
{
	po::options_description desc("Allowed options");
	desc.add_options()
	        ("help", "produce help message")
	        ("input,i", po::value<std::string>()->default_value("-"), "input file")
	        ("output,o", po::value<std::string>()->default_value("-"), "output file")
	        ("split-by-versions,s", "split generated output by version (one module is one version)")
	        ("select", po::value<std::string>()->default_value(""), "produce output only for selected path")
	        ;

	po::variables_map vm;
	po::store(po::parse_command_line(argc,argv, desc), vm);
	po::notify(vm);

	if(vm.count("help")) {
		std::cout << "use this program to prepare module data." << std::endl << desc << std::endl;
		return 1;
	}

	std::string pdata = read_input(vm["input"].as<std::string>());

	modegen::split_by_version splitter;

	try {
		auto result = modegen::parse(pdata);
		if(vm.count("split-by-version")) splitter(result);
		write_output(modegen::converters::to_json(result), vm["output"].as<std::string>());
	}
	catch(const std::runtime_error& ex)
	{
		std::cerr << ex.what() << std::endl;
		return -1;
	}

	return 0;
}
