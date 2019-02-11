#include "filter.hpp"

#include <regex>
#include <algorithm>
#include "parser/interface/helpers.hpp"

namespace mg = modegen::generation::interface;
namespace mi = modegen::parser::interface;

mg::filter::filter(const request& q)
    : query(q)
{
}

std::vector<mi::module>& mg::filter::operator()(std::vector<mi::module>& mods) const
{
	cast_options(mods);
	filter_by_name(mods);
	remove_without_content(mods);

	return mods;
}

void mg::filter::filter_by_name(std::vector<mi::module>& mods) const
{
	const bool need_check_module_name = !query.mod_name.empty();
	const bool need_check_content_name = !query.cnt_name.empty();
	std::regex mname(query.mod_name);
	std::regex cname(query.cnt_name);
	auto cnt_name_check = [&cname](const auto& c){ return std::regex_match(c.name, cname); };
	for(auto& m:mods) {
		const bool entire_module_name_check_fail = need_check_module_name && !std::regex_match(m.name, mname);
		if( entire_module_name_check_fail ) {
			m.content.clear();
			continue;
		}

		auto rpos = std::remove_if(m.content.begin(), m.content.end(), [this](const auto& c){return !is_selected(c, query.sel);} );
		m.content.erase( rpos, m.content.end() );

		if(need_check_content_name) {
			auto rpos = std::remove_if(m.content.begin(), m.content.end(), [&cnt_name_check](const auto& c){return !std::visit(cnt_name_check,c);} );
			m.content.erase( rpos, m.content.end() );
		}
	}
}

void mg::filter::remove_without_content(std::vector<mi::module>& mods) const
{
	auto rpos = std::remove_if(mods.begin(),mods.end(), [](const mi::module& m){return m.content.empty();} );
	mods.erase( rpos, mods.end() );

}

void mg::filter::cast_options(std::vector<mi::module>& mods) const
{
	TODO(uncomment and compile)
	//modegen::gen_options opts = query.opts;
	//if((opts & modegen::gen_options::split_version)==modegen::gen_options::split_version) {
	    //modegen::converters::split_by_version splitter;
	    //splitter(mods);
	//}
}

