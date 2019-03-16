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
		std::unique_ptr<part_descriptor> pdest = part_info(p);
		if(!pdest->need_output()) continue;

		do {
			file_data_ptr tg = prov->generator(cur_filegen(*pdest));
			nlohmann::json data = generate_data(*pdest, *tg);
			if(data.empty()) throw errors::gen_error("common", "no data for output in "s + p);

			tmpl_gen_env gdata(std::move(data), tmpl_path(*pdest));
			gdata.out_file(output_dir / pdest->file_name());
			build_extra_env(gdata, *pdest);
			prov->json_jinja( gdata );
		} while(pdest->next());
	}
}

void mg::generator::generate_stdout(std::string_view part) const
{
	assert( prov );

	std::unique_ptr<part_descriptor> pdest = part_info(part);
	file_data_ptr tg = prov->generator(cur_filegen(*pdest));
	nlohmann::json data = generate_data(*pdest, *tg);

	tmpl_gen_env gdata(std::move(data), tmpl_path(*pdest));
	build_extra_env(gdata, *pdest);
	prov->json_jinja( gdata );
}

nlohmann::json mg::generator::generate_data(const part_descriptor& part, const file_data& fdg) const
{
	assert( prov );

	options::view props(opts, part.part_name());
	return fdg.jsoned_data(prov->parsers(), std::move(props));
}

FS::path mg::generator::tmpl_path(part_descriptor& part) const
{
	assert( prov );
	FS::path input = part.opts().get_opt<std::string>(options::part_option::input).value_or(part.part_name() + u8".jinja"s);
	return prov->resolve_file(input, info_directory, cur_filegen(part));
}

void mg::generator::build_extra_env(tmpl_gen_env& env, const part_descriptor& part) const
{
	assert( opts );

	options::forwards_view fw(opts, part.part_name());
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

std::string mg::generator::cur_filegen(const part_descriptor& part) const
{
	return part.opts().get<std::string>(options::part_option::file_generator);
}

std::unique_ptr<mg::part_descriptor> mg::generator::part_info(std::string_view p) const
{
	assert( opts );
	assert( prov );
	options::view v(opts, p);
	return prov->create_part_descriptor(std::move(v));
}
