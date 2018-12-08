#include <regex>
#include <fstream>
#include <iostream>
#include <functional>

#include <boost/program_options.hpp>
#include <boost/process.hpp>

#include "check.h"
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

std::tuple<po::basic_parsed_options<char>,po::variables_map> parse_command_line(int argc,char** argv)
{
	po::options_description desc("Allowed options");
	desc.add_options()
	        ("help,h", "produce help message")
	        ("input,i", po::value<std::vector<std::string>>(), "input files")
	        ("output,o", po::value<std::vector<std::string>>(), "output files")
	        ("split-by-versions,s", "split generated output by version (one module is one version)")
	        ("select", po::value<std::string>()->default_value(""), "produce output only for selected path")
	        ("target,t", po::value<std::string>()->default_value("server,cpp"), "choice a target, cpp for exmple")
	        ("option,O", po::value<std::string>(), "pass an option to generator")
	        ;
	po::positional_options_description positioned;
	positioned.add("input", -1);

	auto opts = po::command_line_parser(argc,argv).options(desc).positional(positioned).run();

	po::variables_map vm;
	po::store(opts, vm);

	if(vm.count("help")) {
		std::cout << "use this program to prepare module data." << std::endl << desc << std::endl;
		std::exit(1);
	}

	return {opts,vm};
}

std::vector<modegen::module> read_input(const std::vector<std::string>& inputs)
{
	modegen::checker check;
	std::vector<modegen::module> mods;
	auto add_input = [&mods, &check](const std::string& fn){
		auto imods = modegen::parse(read_input(fn));
		check(imods, fn);
		for(auto& im:imods) mods.emplace_back(std::move(im));
	};

	if(inputs.empty()) add_input("-");
	for(auto& i:inputs) add_input(i);

	check(mods);
	return mods;
}

int main(int argc,char** argv)
{
	auto [opts,vm] = parse_command_line(argc, argv);

	// try formats
	// -t server,cpp -Osnake_case -Optr_postfix=_ptr -odeclarations=-
	// -t server,cpp -OcamelCase -oall=some/dir -odeclaration=-
	// -t tests,cpp -oall=some/dir -t bridge,jni -oall=some/other_dir

	std::regex target_match("([a-zA-Z]+)(,([a-zA-Z]+))?", std::regex::egrep);
	std::regex option_match("([a-zA-Z]+)(=(.+))?", std::regex::egrep);

	modegen::generator_maker gmaker;
	modegen::generators::reg_default_generators(&gmaker);

	std::unique_ptr<modegen::generator> cur_gen;
	std::optional<modegen::mod_selection> gen_opts;

	auto mods = read_input(vm["input"].as<std::vector<std::string>>());

	for(auto& opt:opts.options) {
		std::string& key = opt.string_key;

		if(opt.value.empty()) {
			if(gen_opts && key == "split-by-version")
				gen_opts->opts |= modegen::gen_options::split_version;
			continue;
		}

		std::string& val = opt.value[0];
		if(key=="target") {
			if(cur_gen) {
				assert(gen_opts.has_value());
				cur_gen->generate(*gen_opts, mods);
			}

			std::cmatch m;
			std::regex_match(val.data(),m,target_match);
			cur_gen = gmaker.make_generator(m[1].str(),m[3].str());
			if(!cur_gen) std::exit(101);
			gen_opts.reset();
			gen_opts.emplace();
			gen_opts->opts = modegen::gen_options::no_opts;
			gen_opts->sel = modegen::module_content_selector::all;
		}
		else if(key=="option") {
			if(!cur_gen) std::exit(100);
			std::cmatch m;
			std::regex_match(val.data(),m,option_match);
			cur_gen->set_option(m[1].str(), 3 < m.size() ? m[3].str() : "");
		}
		else if(key=="output") {
			if(!cur_gen) std::exit(100);
			assert(gen_opts.has_value());

			std::cmatch m;
			std::regex_match(val.data(),m,option_match);
			std::cout << val << " " << m[1] << " " << m[2] << " " << m[3] << std::endl;
			gen_opts->what_generate = m[1];
			if(m[3]!="-") gen_opts->output = m[3];
		}
	}

	// last generator
	if(cur_gen) {
		assert(gen_opts.has_value());
		cur_gen->generate(*gen_opts, mods);
	} else {
		std::exit(102);
	}

	return 0;
}
