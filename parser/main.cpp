#include <fstream>
#include <iostream>
#include <functional>

#include <boost/program_options.hpp>
#include <boost/process.hpp>

#include "modegen.hpp"
#include "grammar.hpp"
#include "generator.hpp"
#include "generators/defaults.h"

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
		std::fstream file(file_name, file.out);
		file << mods;
		file.flush();
	}
}

int main(int argc,char** argv)
{
	po::options_description desc("Allowed options");
	desc.add_options()
	        ("help,h", "produce help message")
	        ("input,i", po::value<std::vector<std::string>>(), "input file")
	        ("output,o", po::value<std::string>()->default_value("-"), "output file")
	        ("split-by-versions,s", "split generated output by version (one module is one version)")
	        ("select", po::value<std::string>()->default_value(""), "produce output only for selected path")
	        ("target,t", po::value<std::string>()->default_value("cpp"), "choice a target, cpp for exmple")
	        ("generator,g", po::value<std::string>()->default_value(""), "choice a generator")
	        ;

	po::variables_map vm;
	po::store(po::parse_command_line(argc,argv, desc), vm);
	po::notify(vm);

	if(vm.count("help")) {
		std::cout << "use this program to prepare module data." << std::endl << desc << std::endl;
		return 1;
	}

	std::string pdata;
	std::vector<std::string> inputs_files = vm["input"].as<std::vector<std::string>>();
	for(auto& i:inputs_files) pdata += read_input(i);
	if(inputs_files.empty()) pdata = read_input("-");

	modegen::generator_maker gmaker;
	modegen::generators::reg_default_generators(&gmaker);

	modegen::gen_options opts = modegen::gen_options::no_opts;
	if(vm.count("split-by-version")) opts |= modegen::gen_options::split_version;

	try {
		auto gen = gmaker.make_generator(vm["target"].as<std::string>(), vm["generator"].as<std::string>());
		if(!gen) return -1;

		gen->options(opts);
		std::string output = vm["output"].as<std::string>();
		if(output != "-") gen->output(output);

		auto result = modegen::parse(pdata);
		gen->generate(result);
	}
	catch(const std::runtime_error& ex)
	{
		std::cerr << ex.what() << std::endl;
		return -1;
	}

	return 0;
}
