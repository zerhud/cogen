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

