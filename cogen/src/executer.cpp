/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "executer.hpp"

#include <regex>
#include <cassert>
#include <iostream>
#include <boost/property_tree/info_parser.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/karma_stream.hpp>

#include "ix3/utils/to_generic_ast.hpp"
#include "standard_types/src/loader.hpp"
#include "builders/src/loader.hpp"

#include "utils/generic_dsl.hpp"

#include "cppjinja/evtree/evtree.hpp"
#include "cppjinja/parser/parse.hpp"
#include "cppjinja/loader/parser.hpp"
#include "jinja_json_prov.hpp"
#include "flist_provider.hpp"

using namespace cogen;
using cogen::path_config;
using cogen::program_configuration;
using namespace std::literals;
namespace fs = std::filesystem;
namespace po = boost::program_options;

executer::executer(program_configuration pc, int argc, char** argv)
    : main_opts("cogen options")
    , flist_opts("flist mod options")
    , config(std::move(pc))
{
	assert(0 < argc);
	set_options();

	auto opts = po::command_line_parser(argc, argv)
	        .options(common_opts)
	        .run();
	po::store(opts, opt_vars);
}

void executer::set_options()
{
	main_opts.add_options()
	    ("help,h", po::value<std::string>()->implicit_value(""), "produce this help message")
	    ("version", "produce version message")
	    ("help_pathes", po::value<std::string>(), "print pathes for item (can to be input, generators and libraries)")
	    ("which_g", po::value<std::string>(), "try to find path to generator file")
	    ("which_t", po::value<std::string>(), "try to find path to template file")
	    ("outdir,o", po::value<std::string>()->default_value("."), "directory or file where to output")
	    ("generator,g", po::value<std::string>(), "generator (info file)")
	    ("gmode,m", po::value<std::string>()->default_value("json"), "generation mode (\"json\" for generate json and \"dir\" to generate files)")
	    ("input,i", po::value<std::vector<std::string>>(), "input (foramt like -iix3=some_file). use - for read from std input")
	    ("include,I", po::value<std::vector<std::string>>(), "search path for input dsl (common for all inputs)")
	    ("types", po::value<std::string>()->default_value("standard_types.info"), "file for translate types for different languages")
	    ;
	flist_opts.add_options()
	    ("sep", po::value<std::string>()->default_value("\n", "\\n"), "separator for file list")
	    ("prefix", po::value<std::string>()->default_value(""), "prefix for each file")
	    ("postfix", po::value<std::string>()->default_value(""), "postfix for each file")
	    ;
	common_opts.add(main_opts).add(flist_opts);
}

int executer::operator()()
{
	if(execute_commands()) std::exit(0);
	if(!can_continue())
		print_help(), std::exit(0);
	cogen::ic::ptsetts setts(load_settings());
	load_inludes();
	load_inputs();
	load_generic_inputs(setts);
	if(opt_vars["gmode"].as<std::string>()=="json")
		json_mode(setts);
	else if(opt_vars["gmode"].as<std::string>()=="dir")
		dir_mode(setts);
	else if(opt_vars["gmode"].as<std::string>()=="flist")
		flist_mode(setts);
	else {
		std::cerr
			<< "wrong generation mode "
			<< std::quoted(opt_vars["gmode"].as<std::string>())
			<< std::endl;
		return 1;
	}

	return 0;
}

bool executer::execute_commands() const
{
	const bool cmd_ver = 0 != opt_vars.count("version");
	const bool cmd_pth = 0 != opt_vars.count("help_pathes");
	const bool cmd_flg = 0 != opt_vars.count("which_g");
	const bool cmd_flt = 0 != opt_vars.count("which_t");
	if(cmd_pth) config.pathes.print_pathes(std::cout, print_path_which());
	else if(cmd_ver) std::cout << config.version << std::endl;
	else if(cmd_flg) std::cout << config.pathes.generator(opt_vars["which_g"].as<std::string>()) << std::endl;
	else if(cmd_flt) std::cout << config.pathes.library(opt_vars["which_t"].as<std::string>()) << std::endl;
	return cmd_ver || cmd_pth || cmd_flg || cmd_flt;
}

bool executer::can_continue() const
{
	return 0 == opt_vars.count("help")
	    && 0 == opt_vars.count("version")
	    && 0 == opt_vars.count("help_pathes")
	    && 0 <  opt_vars.count("input")
	    && 1 == opt_vars.count("generator")
	        ;
}

boost::property_tree::ptree executer::load_settings() const
{
	auto file = config.pathes.generator(opt_vars["generator"].as<std::string>()+".info"s);
	boost::property_tree::ptree setts_tree;
	boost::property_tree::read_info(file.string(), setts_tree);
	return setts_tree;
}

void executer::load_generic_inputs(const ic::ptsetts& setts)
{
	auto langs = setts.langs();
	for(auto& lang:langs) {
		auto dll_file = config.pathes.generator(lang);
		boost::dll::shared_library& lib = lang_libs.emplace_back(dll_file);
		if(!lib)
			throw std::runtime_error("cannot load library " + std::string(lang));
		auto creator = lib.get<gen_utils::generic_sdl_factory*()>("create_dsl");
		if(!creator)
			throw std::runtime_error("cannot load library's function " + std::string(lang));
		gen_utils::generic_sdl_factory* factory = creator();
		assert(factory);
		auto loaders = factory->languages(
		            [this](const auto& f){return config.pathes.input_data(f);});
		for(auto& l:loaders) load_generic_inputs(*l);
	}
}

void executer::load_generic_inputs(gen_utils::generic_sdl& loader)
{
	std::regex key_val_parser("([0-9a-zA-Z_.]+)(=(.+))?", std::regex::egrep);
	for(auto& input:opt_vars["input"].as<std::vector<std::string>>()) {
		std::cmatch m;
		std::regex_match(input.data(), m, key_val_parser);
		if(m[1].str() == loader.name())
			loader.add(config.pathes.input_data(m[3].str()));
	}
	if(!loader.empty()) {
		loader.finish_loads();
		user_data.add(loader.data());
	}
}

void executer::load_inputs()
{
	ix3::utils::ix3_loader ix3ldr([this](const auto& f){return config.pathes.input_data(f);});
	load_generic_inputs(ix3ldr);

	std_types::loader st;
	user_data.add( st.load_types(config.pathes.library(opt_vars["types"].as<std::string>())) );
}

void executer::load_inludes()
{
	if(opt_vars.count("include"))
		for(auto& inc:opt_vars["include"].as<std::vector<std::string>>())
			config.pathes.add_input_data(inc);
}

void executer::dir_mode(const ic::ptsetts& setts) const
{
	boost::json::value data = create_json(setts).result();
	for(auto& part_data:data.as_array()) {
		cppjinja::evtree ev;
		cppjinja::parser jparse{{}};
		boost::json::object& pd = part_data.as_object();
		std::cout << "parse " << pd["file"] << std::endl;
		jparse.parse(pd["file"].as_string().c_str());
		for(auto& t:jparse.tmpls()) ev.add_tmpl(t);
		ev.render(
		        *create_out_file(pd["out_file"].as_string().c_str()),
		        jinja_json_prov(pd["data"]),
		        pd["out_file"].as_string().c_str());
	}
}

std::unique_ptr<std::ostream> executer::create_out_file(std::string fn) const
{
	auto ret = std::make_unique<std::fstream>(fn);
	return ret;
}

void executer::flist_mode(const cogen::ic::ptsetts& setts) const
{
	using namespace boost::spirit;
	using namespace boost::spirit::standard;

	auto sep = opt_vars["sep"].as<std::string>();
	auto pref = opt_vars["prefix"].as<std::string>();
	auto suff = opt_vars["postfix"].as<std::string>();
	flist_provider flp(config.pathes);
	generate_to_provider(flp, setts);
	if(sep.empty()) sep = "\n";
	std::cout << karma::format((lit(pref) << *char_ << lit(suff)) % sep, flp.result()) << std::flush;
}

void executer::json_mode(const ic::ptsetts& setts) const
{
	std::cout << create_json(setts).result() << std::endl;
}

void executer::generate_to_provider(ic::provider& prov, const ic::ptsetts& setts) const
{
	cogen::ic::single_gen_part part(&prov);
	cogen::ic::gen_context ctx;
	builders::loader bld_ldr;
	for(auto& pname:setts.parts()) {
		ctx.cfg_part = setts.part_setts(pname);
		auto pd = user_data;
		pd.add(setts.generic_ast(pname));
		if(auto bld = bld_ldr(setts.part_src(pname), ctx);bld)
			pd.add(*bld);
		ctx.generated[pname] = part(ctx, std::move(pd));
	}
}

json_provider executer::create_json(const ic::ptsetts& setts) const
{
	json_provider json_out(config.pathes);
	json_out.output_dir(opt_vars["outdir"].as<std::string>());
	generate_to_provider(json_out, setts);
	return json_out;
}

cogen::avaible_pathes executer::print_path_which() const
{
	assert(0 != opt_vars.count("help_pathes"));
	std::string val = opt_vars["help_pathes"].as<std::string>();
	if(val == "input") return cogen::avaible_pathes::input;
	if(val == "generators") return cogen::avaible_pathes::generators;
	if(val == "libraries") return cogen::avaible_pathes::libraries;
	throw std::runtime_error("no such path avaible " + val);
}

void executer::print_help() const
{
	std::cout
		<< "version: " << config.version << std::endl
		<< "this is a source code generator. use with options"
		<< std::endl
		<< common_opts
		<< std::endl;
}
