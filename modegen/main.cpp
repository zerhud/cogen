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

#include "pg/provider.hpp"
#include "pg/part_algos/module.hpp"

#include "parser/interface/loader.hpp"

#include "pythongen.hpp"

namespace mg = modegen::pg;
namespace mi = modegen::parser::interface;
namespace po = boost::program_options;

using namespace std::literals;

class gen_prov : public mg::provider
{
public:
	gen_prov(FS::path self_path)
	{
		(void)self_path;
	}

	std::unique_ptr<mg::part_algos> create_algos(mg::input_lang il) const override
	{
		if(il==mg::input_lang::module) return std::make_unique<mg::palgos::module_algos>();
		return nullptr;
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

	mg::output_descriptor_ptr craete_output(mg::output_lang lng, FS::path p) const override
	{
		throw std::runtime_error("no such generator was loaded \""s + mg::to_string(lng) + "\""s);
	}

	void generate_from_jinja(const mg::jinja_env& env) const override
	{/*
		nlohmann::json json_data;

		auto fnc_list = data.emb_fnc_list();
		for(auto& ef:fnc_list) {
			if(std::holds_alternative<std::string>(ef.second)) {
				json_data["extra_data"][ef.first]["name"] = ef.first;
				json_data["extra_data"][ef.first]["script"] = std::get<std::string>(ef.second);
			}
			else if(std::holds_alternative<FS::path>(ef.second)) {
				json_data["extra_data"][ef.first]["name"] = ef.first;
				json_data["extra_data"][ef.first]["file"] = std::get<FS::path>(ef.second).u8string();
			}
		}

		mg::python_evaluator ev(data.data());
		ev
		        .sys_path("some/path"s)
		        .script(data.exec_before())
		        ;

		auto emb_fnc_list = data.emb_fnc_list();
		for(const auto& ef:emb_fnc_list) {
			ev.add_emb_fnc(ef.first, ef.second);
		}

		ev
		        .tmpl(data.tmpl(), data.out_file())
		        .script(data.exec_after())
		        ;
	*/}

	void add_search_path(const FS::path& p)
	{
		search_pathes.push_back(p);
	}

	/*FS::path resolve_file(const FS::path& p, const FS::path& assumed, std::string_view gen_name) const override
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
		ret = resolve_file(p.generic_u8string() + u8".info", final_search);
		if(ret) return *ret;
		ret = resolve_file(p.generic_u8string() + u8".jinja", final_search);
		if(ret) return *ret;

		std::string err_msg = u8"cannot find file "s + p.generic_u8string() + u8"\ntry to search in:\n"s;
		for(const auto& sp:final_search) err_msg += u8"\t" + sp.generic_u8string() + u8"\n"s;
		throw std::runtime_error(err_msg);
	}*/

	std::unique_ptr<mg::part_descriptor> create_part(mg::options::part_view&& v) const override
	{
		auto ng = v.get_opt<std::string>(mg::options::part_option::output_name_gen).value_or("single");
		if(ng=="interface"s) return std::make_unique<mg::interface::part_descriptor>(v, parsers());
		if(ng=="interface,unite"s) return std::make_unique<mg::interface::part_descriptor>(v ,parsers(), true);
		return std::make_unique<mg::single_part_descriptor>(std::move(v), parsers());
	}

	std::vector<std::string> list_target() const
	{
		return lman.name_list();
	}

	std::vector<std::string> list_generators() const
	{
		return { u8"cpp"s, u8"cmake"s, u8"py"s };
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
			gen = std::make_unique<mg::generator>(prov, info_path.parent_path());
			boost::property_tree::read_info(info_path.u8string(), gen->options());
		}
		else if(key=="option"sv) {
			if(!gen) throw std::runtime_error("cannot override option without generator");

			std::cmatch m;
			std::regex_match(val.data(), m, key_val_parser);
			gen->options().put(m[1].str(),m[3].str());
		}
		else if(key=="aoption"sv) {
			if(!gen) throw std::runtime_error("cannot add option without generator");
			gen->options().add(key,val);
		}
		else if(key=="include"sv) {
			prov->add_search_path(val);
		}
		else if(key=="outdir"sv) {
			out_dir = val;
			if(!FS::exists(out_dir)) FS::create_directories(out_dir);
		}
		else {
			std::cerr << u8"unknown option "sv << key << "="sv << val << std::endl;
		}
	}

	if(gen) gen->generate(out_dir);

	return 0;
}

