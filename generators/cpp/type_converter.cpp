#include "type_converter.h"

#include "helpers.hpp"

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
};

modegen::helpers::type_converter::type_converter(module_content_selector s, std::vector<modegen::module>& mods)
    : selector(s)
{
	auto v = [this](auto& mc) { convert(mc); };
	for(auto& mod:mods) for(auto& mc:mod.content) if(is_selected(mc, selector)) std::visit(v, mc);
}

void modegen::helpers::type_converter::convert(modegen::function& obj) const
{
	convert(obj.return_type);
	for(auto& p:obj.func_params) convert(p.param_type);
}

void modegen::helpers::type_converter::convert(modegen::record& obj) const
{
	for(auto& m:obj.members) convert(m.param_type);
}

void modegen::helpers::type_converter::convert(modegen::enumeration& /*obj*/) const
{
}

void modegen::helpers::type_converter::convert(modegen::interface& obj) const
{
	for(auto& f:obj.mem_funcs) convert(f);
	for(auto& c:obj.constructors) for(auto cp:c.func_params) convert(cp.param_type);
}

void modegen::helpers::type_converter::convert(modegen::type& t) const
{
	auto npos = type_maps.find(t.name);
	if(npos!=type_maps.end()) t.name = npos->second;

	for(auto& s:t.sub_types) convert(s);
}
