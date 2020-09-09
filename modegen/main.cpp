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
#include <boost/program_options.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/process.hpp>
#include <pybind11/embed.h>
#include <cppjinja/loader/parser.hpp>

#include "config.hpp"
#include "pg/provider.hpp"
#include "pg/part_algos/module.hpp"
#include "pg/part_algos/data.hpp"
#include "pg/generator.hpp"
#include "pg/options.hpp"
#include "pg/info_part.hpp"
#include "pg/langs/cpp.hpp"
#include "pg/langs/python.hpp"
#include "pg/langs/cmake.hpp"

#include "parser/interface/loader.hpp"
#include "parser/interface/ix3_loader.hpp"

#include "pythongen.hpp"

namespace mg = modegen::pg;
namespace mpi = modegen::parser::interface;
namespace po = boost::program_options;

using namespace std::literals;

class gen_prov : public mg::provider, public std::enable_shared_from_this<gen_prov>
{
	cppjinja::parser cpjp{{"."}};
public:
	gen_prov(FS::path self_path)
	{
		(void)self_path;
	}

	std::unique_ptr<mg::configuration_algos> create_algos(mg::input_lang il) const override
	{
		if(il==mg::input_lang::mdl) return std::make_unique<mg::palgos::interface_conf_algos>(input());
		if(il==mg::input_lang::data) return std::make_unique<mg::palgos::data_algos>(input());
		assert(false);
		throw std::runtime_error("language algos not implemented for "s + to_string(il));
	}

	modegen::parser::loader_ptr create_loader(std::string_view target, FS::path input)
	{
		auto ldr = lman.require(target);

		assert(ldr);

		if(input=="-"sv) ldr->load(std::cin, "stdin");
		else ldr->load(input);

		return ldr;
	}

	std::vector<modegen::parser::loader_ptr> input() const override
	{
		// nonconst only first time called
		return const_cast<decltype(lman)&>(lman).finish_loads();
	}

	mg::output_descriptor_ptr create_output(mg::output_lang lng, FS::path p, std::vector<std::any> data) const override
	{
		if(lng==mg::output_lang::cpp) return std::make_shared<mg::outputs::cpp>(std::move(p), std::move(data));
		if(lng==mg::output_lang::cmake) return std::make_shared<mg::outputs::cmake>(std::move(p), std::move(data));
		throw std::runtime_error("no such generator was loaded \""s + mg::to_string(lng) + "\""s);
	}

	void generate_from_jinja(const mg::jinja_env& env) const override
	{
		nlohmann::json json_data;

		//auto fnc_list = data.emb_fnc_list();
		//for(auto& ef:fnc_list) {
			//if(std::holds_alternative<std::string>(ef.second)) {
				//json_data["extra_data"][ef.first]["name"] = ef.first;
				//json_data["extra_data"][ef.first]["script"] = std::get<std::string>(ef.second);
			//}
			//else if(std::holds_alternative<FS::path>(ef.second)) {
				//json_data["extra_data"][ef.first]["name"] = ef.first;
				//json_data["extra_data"][ef.first]["file"] = std::get<FS::path>(ef.second).string();
			//}
		//}

		modegen::generation::python_evaluator ev(env.data);
		ev
		        .sys_path("some/path"s)
		        //.script(data.exec_before())
		        ;

		//auto emb_fnc_list = data.emb_fnc_list();
		//for(const auto& ef:emb_fnc_list) {
			//ev.add_emb_fnc(ef.first, ef.second);
		//}

		ev
		        .tmpl(env.tmpl, env.out_file)
		        //.script(data.exec_after())
		        ;
	}

	void add_search_path(const FS::path& p)
	{
		search_pathes.push_back(p);
	}

	FS::path resolve_file(const FS::path& p, const FS::path& assumed, mg::output_lang lng) const override
	{
		return resolve_file(p, assumed, mg::to_string(lng));
	}

	FS::path resolve_file(const FS::path& p, const FS::path& assumed, std::string_view gen_name) const
	{
		if(p.is_absolute()) return p;

		std::vector<FS::path> final_search;
		final_search.reserve(search_pathes.size()*2 + 2);
		if(!assumed.empty()) {
			final_search.push_back(assumed);
			if(!gen_name.empty()) final_search.push_back(assumed / gen_name);
		}

		final_search.push_back(FS::current_path());
		for(const auto& sp:search_pathes) final_search.push_back(sp);

		if(!gen_name.empty()) {
			for(const auto& sp:search_pathes) final_search.push_back(sp / gen_name);
		}

		auto ret = resolve_file(p, final_search);
		if(ret) return *ret;
		ret = resolve_file(p.generic_string() + ".info"s, final_search);
		if(ret) return *ret;
		ret = resolve_file(p.generic_string() + ".jinja"s, final_search);
		if(ret) return *ret;

		std::string err_msg = "cannot find file "s + p.generic_string() + "\ntry to search in:\n"s;
		for(const auto& sp:final_search) err_msg += "\t"s + sp.generic_string() + "\n"s;
		throw std::runtime_error(err_msg);
	}

	mg::part_descriptor_ptr create_part(mg::options::part_view&& v) const override
	{
		return std::make_shared<mg::info_part>(std::move(v));
	}

	std::vector<std::string> list_target() const
	{
		return lman.name_list();
	}

	std::vector<std::string> list_generators() const
	{
		return { "cpp"s, "cmake"s, "py"s };
	}
private:
	std::optional<FS::path> resolve_file(FS::path p, const std::vector<FS::path> final_search) const
	{
		std::optional<FS::path> ret;
		for(auto& sp:final_search) {
			ret = sp / p;
			if(FS::exists(*ret)) return ret;
		}

		return ret;
	}

	modegen::parser::loaders_manager lman;
	std::vector<FS::path> search_pathes;
};

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

int main(int argc, char** argv)
{
	auto prov = std::make_shared<gen_prov>(argv[0]);
	prov->add_search_path(modegen::settings::templates_dir);

	auto [opts,vm] = parse_command_line(argc, argv, prov->list_target(), prov->list_generators());

	std::unique_ptr<mg::generator> gen;
	FS::path out_dir = FS::current_path();
	std::regex key_val_parser("([0-9a-zA-Z_.]+)(=(.+))?", std::regex::egrep);
	for(auto& opt:opts.options) {
		std::string& key = opt.string_key;
		std::string& val = opt.value[0];
		if(key=="input"sv) {
			std::cmatch m;
			std::regex_match(val.data(), m, key_val_parser);
			FS::path input_path = m[3].str();
			if(input_path.is_relative()) input_path = FS::current_path() / input_path;
			prov->create_loader(m[1].str(), input_path);
		}
		else if(key=="generator"sv) {
			if(gen) gen->generate(out_dir);

			FS::path info_path = prov->resolve_file(val, "", "");
			auto opts = std::make_shared<mg::options::container>(info_path);
			gen = std::make_unique<mg::generator>(prov, opts);
			boost::property_tree::read_info(info_path.string(), opts->raw());
		}
		else if(key=="option"sv) {
			if(!gen) throw std::runtime_error("cannot override option without generator");

			std::cmatch m;
			std::regex_match(val.data(), m, key_val_parser);
			gen->opts()->raw().put(m[1].str(),m[3].str());
		}
		else if(key=="aoption"sv) {
			if(!gen) throw std::runtime_error("cannot add option without generator");
			gen->opts()->raw().add(key,val);
		}
		else if(key=="include"sv) {
			prov->add_search_path(val);
		}
		else if(key=="outdir"sv) {
			out_dir = val;
			if(!FS::exists(out_dir)) FS::create_directories(out_dir);
		}
		else {
			std::cerr << "unknown option "sv << key << "="sv << val << std::endl;
		}
	}

	if(gen) gen->generate(out_dir);

	return 0;
}

