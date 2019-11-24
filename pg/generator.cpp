/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "generator.hpp"

#include <cassert>

#include "options.hpp"
#include "provider.hpp"
#include "exceptions.hpp"
#include "output_configurator.hpp"
#include "output_descriptor.hpp"

namespace mpg = modegen::pg;
using namespace std::literals;

mpg::generator::generator(mpg::provider_ptr p, mpg::options::container_ptr s)
	: prov(std::move(p))
	, setts(std::move(s))
{
}

mpg::options::container_ptr mpg::generator::opts() const
{
	assert(setts);
	return setts;
}

/// builds all parts.
/// after building all part are ready for generate output.
void mpg::generator::build_env()
{
	init_parts();
	build_outs();
}

mpg::part_manager& mpg::generator::parts()
{
	return pman;
}

void mpg::generator::generate(const FS::path& output_dir)
{
	assert(prov);
	build_env();

	if(!FS::exists(output_dir)) FS::create_directories(output_dir);
	else if(!FS::is_directory(output_dir)) throw errors::error("you must specify directory for output");

	auto plist = pman.list();
	for(auto& part:plist) {
		jinja_env env;
		env.tmpl = part->tmpl_file();
		auto olist = part->outputs();
		for(auto& out:olist) {
			env.data = out->data(pman);
			env.out_file = output_dir / out->file();
			prov->generate_from_jinja(env);
		}
	}
}

void mpg::generator::init_parts()
{
	assert(prov);
	assert(setts);

	auto plist = setts->part_list();
	for(auto&& p:plist) {
		options::part_view psetts(setts,p);

		auto part = prov->create_part(std::move(psetts));
		if(part==nullptr) throw errors::error("part "s + p + " cannot be created"s);

		pman.add(std::move(part));
	}
}

void mpg::generator::build_outs()
{
	auto plist = pman.list();
	for(auto& p:plist) {
		assert(p);
		p->build_outputs(prov);
	}
}

