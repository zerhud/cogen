#include "generator.hpp"

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

static void remove_empty(std::vector<modegen::module>& mods)
{
	auto rpos = std::remove_if(mods.begin(),mods.end(), [](const modegen::module& m){return m.content.empty();} );
	mods.erase( rpos, mods.end() );

}

void modegen::filter_by_selection(const modegen::mod_selection& query, std::vector<modegen::module>& mods)
{
	cast_options(query.opts, mods);

	for(auto& m:mods) {
		auto rpos = std::remove_if(m.content.begin(), m.content.end(), [&query](const auto& c){return !is_selected(c, query.sel);} );
		m.content.erase( rpos, m.content.end() );
	}

	remove_empty(mods);
}
