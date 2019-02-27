/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "common.hpp"

#include "provider.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;

mg::generator::generator(mg::provider_ptr p, const FS::path& i)
	: prov(std::move(p))
	, info_directory(i)
{
	if(!prov) throw errors::gen_error("common"s, "cannot create generator without provider"s);
}

boost::property_tree::ptree& mg::generator::options()
{
	return opts;
}

const boost::property_tree::ptree& mg::generator::options() const
{
	return opts;
}

void mg::generator::generate(const FS::path& output_dir) const
{
	assert( prov );
	for(auto& part:opts.get_child("gen")) {
		nlohmann::json data = generate_data(part.first);
		if(data.empty()) throw errors::gen_error("common", "no data for output");

		tmpl_gen_env gdata(std::move(data), tmpl_path(part.first));
		gdata.out_dir(output_dir / output_path(part.first));
		build_extra_env(gdata, extra_generator_data(part.first), part.first);
		prov->json_jinja( gdata );
	}
}

void mg::generator::generate_stdout(std::string_view part) const
{
	assert( prov );
	nlohmann::json data = generate_data(part);
	tmpl_gen_env gdata(std::move(data), tmpl_path(part));
	build_extra_env(gdata, extra_generator_data(part), part);
	prov->json_jinja( gdata );
}

nlohmann::json mg::generator::generate_data(std::string_view part) const
{
	assert( prov );

	std::vector<std::string> parsers_names = parser_name_list(part);
	std::vector<parser::loader_ptr> parsers;
	for(auto& pn:parsers_names) parsers.emplace_back(prov->parser(pn));

	auto tg = prov->generator(cur_target(part));

	assert(tg);
	options_view props(opts, part);
	return tg->jsoned_data(std::move(parsers), std::move(props));
}

std::vector<std::string> mg::generator::parser_name_list(std::string_view part) const
{
	std::vector<std::string> ret;
	options_view opts_view(opts, part);
	return opts_view.part_str_list(""s, "parser"s);

	return ret;
}

FS::path mg::generator::output_path(std::string_view part) const
{
	std::string p(part);
	std::string path = "gen." + p + ".output";
	return opts.get(path, p + u8".hpp");
}

FS::path mg::generator::tmpl_path(std::string_view part) const
{
	assert( prov );

	std::string p(part);
	std::string path = "gen." + p + ".input";
	FS::path input_file = opts.get(path, p + u8".jinja");

	return prov->resolve_file(input_file, info_directory, cur_target(part));
}

boost::property_tree::ptree mg::generator::extra_generator_data(std::string_view part) const
{
	boost::property_tree::ptree ret;
	std::string p(part);
	auto forwards = opts.get_child_optional("gen."s+p+".forwards"s);
	if(!forwards) forwards = opts.get_child_optional("defaults.forwards"s);
	if(forwards) ret = std::move(*forwards);
	return ret;
}

void mg::generator::build_extra_env(tmpl_gen_env& env, const boost::property_tree::ptree& ex_data, std::string_view part) const
{
	auto ex_child = ex_data.get_child_optional("ex"s);
	if(ex_child) for(auto& ex:*ex_child) {
		auto name = ex.second.get<std::string>("name"s);
		auto file = ex.second.get_optional<std::string>("file"s);
		auto script = ex.second.get_optional<std::string>("script"s);
		if(script) env.emb_fnc(name, *script);
		else if(file) env.emb_fnc(name, prov->resolve_file(*file, info_directory, cur_target(part)));
		else throw errors::gen_error("common"s, "for embade function you must specify the name and script or file options"s);
	}

	auto before_script = ex_data.get_optional<std::string>("before.script"s);
	auto before_file = ex_data.get_optional<std::string>("before.file"s);
	auto after_script = ex_data.get_optional<std::string>("after.script"s);
	auto after_file = ex_data.get_optional<std::string>("after.file"s);

	if(before_script) env.exec_before(*before_script);
	else if(before_file) env.exec_before(FS::path(*before_file));

	if(after_script) env.exec_after(*after_script);
	else if(after_file) env.exec_after(*after_file);
}

std::string mg::generator::cur_target(std::string_view part) const
{
	std::string p(part);
	return opts.get("gen."s+p+".target"s,opts.get("defaults.target"s,""s));
}
