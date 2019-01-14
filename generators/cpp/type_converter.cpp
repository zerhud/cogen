#include "type_converter.h"

#include "parser/helpers.hpp"
#include "parser/errors.h"

std::map<std::string,std::string> modegen::helpers::type_converter::type_maps =
{
      {"i8","std::int8_t"}
    , {"i16","std::int16_t"}
    , {"i32","std::int32_t"}
    , {"i64","std::int64_t"}
    , {"f32","float"}
    , {"f64","double"}
    , {"string","std::string"}
    , {"date","std::chrono::system_clock::time_point"}
    , {"binary","std::vector<std::byte>"}
    , {"list","std::vector"}
    , {"map","std::map"}
    , {"optional","std::optional"}
    , {"void","void"}
};

std::map<std::string,std::string> modegen::helpers::type_converter::incs_maps =
{
      {"i8","cstdint"}
    , {"i16","cstdint"}
    , {"i32","cstdint"}
    , {"i64","cstdint"}
    , {"string","string"}
    , {"date","chrono"}
    , {"binary","vector"}
    , {"list","vector"}
    , {"map","map"}
    , {"optional","optional"}
};

modegen::helpers::type_converter::type_converter(std::vector<modegen::module>& mods)
{
	auto v = [this](auto& mc) { convert(mc); };
	for(auto& mod:mods) {
		cur_mod = &mod;
		for(auto& mc:mod.content) std::visit(v, mc);

		std::sort(cur_mod->imports.begin(),cur_mod->imports.end(),[](using_directive& left, using_directive& right){return left.mod_name < right.mod_name;});
		auto upos = std::unique(cur_mod->imports.begin(),cur_mod->imports.end(),
		                        [](using_directive& left, using_directive& right){return left.mod_name==right.mod_name;});
		cur_mod->imports.erase(upos,cur_mod->imports.end());

		for(auto& i:cur_mod->imports) if(i.is_system) total_incs.emplace_back(i.mod_name);
	}

	std::sort(total_incs.begin(),total_incs.end());
	total_incs.erase(std::unique(total_incs.begin(),total_incs.end()), total_incs.end());

	cur_mod = nullptr;
}

std::vector<std::string> modegen::helpers::type_converter::includes() const
{
	return total_incs;
}

void modegen::helpers::type_converter::convert(modegen::function& obj) const
{
	convert(obj.return_type);
	for(auto& p:obj.func_params) convert(p.param_type);
}

void modegen::helpers::type_converter::convert(modegen::record& obj) const
{
	assert(cur_mod);
	cur_mod->imports.emplace_back(modegen::using_directive{"memory"});
	for(auto& m:obj.members) convert(m.param_type);

	solve_type(obj.name, modegen::module_content_selector::record);
}

void modegen::helpers::type_converter::convert(modegen::enumeration& obj) const
{
	solve_type(obj.name, modegen::module_content_selector::enumeration);
}

void modegen::helpers::type_converter::convert(modegen::interface& obj) const
{
	assert(cur_mod);

	cur_mod->imports.emplace_back(modegen::using_directive{"memory"});
	for(auto& f:obj.mem_funcs) convert(f);
	for(auto& c:obj.constructors) {
		for(auto& cp:c.func_params) {
			convert(cp.param_type);
		}
	}

	solve_type(obj.name, modegen::module_content_selector::interface);
}

void modegen::helpers::type_converter::convert(modegen::type& t) const
{
	assert(cur_mod);

	auto ipos = incs_maps.find(t.name);
	if(ipos!=incs_maps.end()) cur_mod->imports.emplace_back(using_directive{ipos->second,true});

	auto npos = type_maps.find(t.name);
	if(npos!=type_maps.end()) t.name = npos->second;
	else {
		if(!t.sub_types.empty()) throw errors::error("cpp_type_cvt", "defined type cannot have parameters");
		defined_types.emplace_back(type_info{t});
	}

	for(auto& s:t.sub_types) convert(s);
}

void modegen::helpers::type_converter::solve_type(std::string_view name, modegen::module_content_selector from) const
{
	for(auto& t:defined_types) if(t.type.name == name) t.points = from;
}
