/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "common.hpp"

#include "provider.hpp"
#include "part_descriptor.hpp"
#include "output_info.hpp"
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

	output_info outputs;
	for(auto& p:plist) outputs.add_part(prov->create_part_descriptor(options::view(opts, p)));

	while(outputs.next()) {
		options::view& popts = outputs.current_part()->opts();
		file_data_ptr fg = prov->generator(cur_filegen(popts));
	}

	for(auto& p:plist) {
		mg::options::view part_opts(opts, p);
		file_data_ptr tg = prov->generator(cur_filegen(part_opts));
		std::vector<file_data::output_info> info = tg->jsoned_data(prov->parsers(), part_opts);
		for(auto& oi:info) {
			tmpl_gen_env gdata(std::move(oi.data), tmpl_path(part_opts));
			gdata.out_file(output_dir / oi.out_file);
			build_extra_env(gdata, part_opts);
			prov->json_jinja( gdata );
		}
	}
}

void mg::generator::generate_stdout(std::string_view part) const
{
	assert( prov );
	assert( opts );

	mg::options::view part_opts(opts, part);
	file_data_ptr tg = prov->generator(cur_filegen(part_opts));
	std::vector<file_data::output_info> infos = tg->jsoned_data(prov->parsers(), part_opts);
	for(auto& p:infos) {
		tmpl_gen_env gdata(std::move(p.data), tmpl_path(part_opts));
		build_extra_env(gdata, part_opts);
		prov->json_jinja( gdata );
	}
}

FS::path mg::generator::tmpl_path(const options::view& opts) const
{
	assert( prov );
	FS::path input = opts.get_opt<std::string>(options::part_option::input).value_or(std::string(opts.part()) + u8".jinja"s);
	return prov->resolve_file(input, info_directory, cur_filegen(opts));
}

FS::path mg::generator::tmpl_path(const part_descriptor& part) const
{
	return tmpl_path(part.opts());
}

void mg::generator::build_extra_env(tmpl_gen_env& env, const options::view& part_opts) const
{
	assert( opts );

	options::forwards_view fw(opts, part_opts.part());
	auto before = fw.before();
	if(before) env.exec_before(*before);

	auto after = fw.after();
	if(after) env.exec_after(*after);

	auto ex_list = fw.ex_list();
	for(auto ex:ex_list) {
		if(std::holds_alternative<FS::path>(ex.source))
			ex.source = prov->resolve_file(std::get<FS::path>(ex.source), info_directory, cur_filegen(part_opts) );
		env.emb_fnc(ex.name, ex.source);
	}
}

std::string mg::generator::cur_filegen(const options::view& opts) const
{
	return opts.get<std::string>(options::part_option::file_generator);
}

std::unique_ptr<mg::part_descriptor> mg::generator::part_info(std::string_view p) const
{
	assert( opts );
	assert( prov );
	options::view v(opts, p);
	return prov->create_part_descriptor(std::move(v));
}
