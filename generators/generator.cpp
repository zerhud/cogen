#include "generator.hpp"

#include <regex>
#include "split_by_version.hpp"

void modegen::generator_maker::register_gen(std::string_view part, std::string_view name, modegen::generator_creator crt)
{
	auto path = make_path(part,name);
	//if(gens.find(path)!=gens.end()) throw;
	gens[path] = crt;
}

std::unique_ptr<modegen::generator> modegen::generator_maker::make_generator(std::string_view part, std::string_view name) const
{
	auto path = make_path(part,name);
	auto pos = gens.find(path);
	if(pos==gens.end()) return nullptr;
	return pos->second();
}

std::string modegen::generator_maker::make_path(std::string_view part, std::string_view name)
{
	//if(part.find('.')!=std::string::npos) throw;
	//if(name.find('.')!=std::string::npos) throw;
	return std::string(part) + std::string(".") + std::string(name);
}

static void /*modegen::*/cast_options(modegen::gen_options opts, std::vector<modegen::module>& mods)
{
	if((opts & modegen::gen_options::split_version)==modegen::gen_options::split_version) {
		modegen::converters::split_by_version splitter;
		splitter(mods);
	}
}

static void remove_without_content(std::vector<modegen::module>& mods)
{
	auto rpos = std::remove_if(mods.begin(),mods.end(), [](const modegen::module& m){return m.content.empty();} );
	mods.erase( rpos, mods.end() );

}

void modegen::filter_by_selection(const modegen::mod_selection& query, std::vector<modegen::module>& mods)
{
	cast_options(query.opts, mods);


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

		auto rpos = std::remove_if(m.content.begin(), m.content.end(), [&query](const auto& c){return !is_selected(c, query.sel);} );
		m.content.erase( rpos, m.content.end() );

		if(need_check_content_name) {
			auto rpos = std::remove_if(m.content.begin(), m.content.end(), [&cnt_name_check](const auto& c){return !std::visit(cnt_name_check,c);} );
			m.content.erase( rpos, m.content.end() );
		}
	}

	remove_without_content(mods);
}

void modegen::generation::select(modegen::generator_request& req)
{
	filter_by_selection(req.selector, req.data);
}

modegen::generator_request& modegen::generator_request::operator |= (const std::function<void(modegen::generator_request&)>& gen)
{
	return *this | gen;
}

modegen::generator_request& modegen::operator |(modegen::generator_request& req, const std::function<void (modegen::generator_request&)>& gen)
{
	if(gen) gen(req);
	return req;
}
