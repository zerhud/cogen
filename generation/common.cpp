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
	, opts(std::make_shared<options::container>())
	, info_directory(i)
{
	if(!prov) throw errors::gen_error("common"s, "cannot create generator without provider"s);
}

boost::property_tree::ptree& mg::generator::options()
{
	assert(opts);
	return opts->raw();
}

const boost::property_tree::ptree& mg::generator::options() const
{
	assert(opts);
	return opts->raw();
}

void mg::generator::generate(const FS::path& output_dir) const
{
	assert( prov );
	assert( opts );
	auto plist = opts->part_list();
	for(auto& p:plist) {
		nlohmann::json data = generate_data(p);
		if(data.empty()) throw errors::gen_error("common", "no data for output in "s + p);

		tmpl_gen_env gdata(std::move(data), tmpl_path(p));
		gdata.out_file(output_dir / output_path(p));
		build_extra_env(gdata, p);
		prov->json_jinja( gdata );
	}
}

void mg::generator::generate_stdout(std::string_view part) const
{
	assert( prov );
	nlohmann::json data = generate_data(part);
	tmpl_gen_env gdata(std::move(data), tmpl_path(part));
	build_extra_env(gdata, part);
	prov->json_jinja( gdata );
}

nlohmann::json mg::generator::generate_data(std::string_view part) const
{
	assert( prov );

	auto tg = prov->generator(cur_filegen(part));

	assert(tg);
	options::view props(opts, part);
	return tg->jsoned_data(prov->parsers(), std::move(props));
}

FS::path mg::generator::output_path(std::string_view part) const
{
	assert( opts );
	auto val = opts->get_opt<std::string>(options::part_option::output, std::string(part), ""s);
	if(!val) throw errors::gen_error("common"s, "no output file provided for "s + std::string(part));
	return *val;
}

FS::path mg::generator::tmpl_path(std::string_view part) const
{
	assert( prov );
	assert( opts );

	FS::path input_file = opts->get_opt<std::string>(options::part_option::input, std::string(part), ""s).value_or(std::string(part) + u8".jinja"s);
	return prov->resolve_file(input_file, info_directory, cur_filegen(part));
}

void mg::generator::build_extra_env(tmpl_gen_env& env, std::string_view part) const
{
	assert( opts );

	options::forwards_view fw(opts, part);
	auto before = fw.before();
	if(before) env.exec_before(*before);

	auto after = fw.after();
	if(after) env.exec_after(*after);

	auto ex_list = fw.ex_list();
	for(auto ex:ex_list) {
		if(std::holds_alternative<FS::path>(ex.source))
			ex.source = prov->resolve_file(std::get<FS::path>(ex.source), info_directory, cur_filegen(part) );
		env.emb_fnc(ex.name, ex.source);
	}
}

std::string mg::generator::cur_filegen(std::string_view part) const
{
	assert( opts );
	std::string p(part);
	return opts->get<std::string>(options::part_option::file_generator, p, ""s);
}
