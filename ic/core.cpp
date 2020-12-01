/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "core.hpp"

#include <common_utils/input/map_to.hpp>

using namespace std::literals;

modegen::ic::core::core(std::shared_ptr<provider> p)
    : prov(std::move(p))
{
}

void modegen::ic::core::gen(const configuration& config) const
{
	assert(prov);
	gen_utils::map_to mapper;
	auto all_dsl = config.all_dsl();
	for(auto& p:config.parts()) {

		auto map_tmpl = config.map_tmpl(p);
		std::map<std::pmr::string, input> part_outputs;
		for(auto& t:all_dsl.all()) {
			auto result = mapper(map_tmpl, *t);
			for(auto& r:result)
				part_outputs[r.first].add(std::move(r.second));
		}

		for(auto& out:part_outputs) {
			auto result = make_json(config, out.second);
			prov->generate(config.tmpl_file(p), result, out.first);
		}
	}
}

boost::json::value modegen::ic::core::make_json(
		const configuration& config, const input& dsl) const
{
	struct compilconfig : gen_utils::compilation_config {
		gen_utils::compiler compiler_name () const override {return gen_utils::compiler::cpp;}
		std::string_view value(std::string_view key) const override {return ""sv;} ;
		gen_utils::name_conversion naming() const override {return gen_utils::name_conversion::as_is;}
	};
	compilconfig cfg;
	boost::json::array ret;
	for(auto& t:dsl.all())
		ret.emplace_back(t->to_json(cfg));
	return ret;
}
