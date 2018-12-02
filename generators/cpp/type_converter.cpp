#include "type_converter.h"

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
};

modegen::helpers::type_converter::type_converter(module_content_selector s, std::vector<modegen::module>& mods)
    : selector(s)
{
	auto v = [this](auto& mc) { convert(mc); };
	for(auto& mod:mods) for(auto& mc:mod.content) std::visit(v, mc);
}

void modegen::helpers::type_converter::convert(modegen::function& obj) const
{
	if(do_check && (selector & module_content_selector::function) != module_content_selector::function) return;
	convert(obj.return_type);
	for(auto& p:obj.func_params) convert(p.param_type);
}

void modegen::helpers::type_converter::convert(modegen::record& obj) const
{
	if((selector & module_content_selector::record) != module_content_selector::record) return;
	for(auto& m:obj.members) convert(m.param_type);
}

void modegen::helpers::type_converter::convert(modegen::enumeration& /*obj*/) const
{
}

void modegen::helpers::type_converter::convert(modegen::interface& obj) const
{
	if((selector & module_content_selector::record) != module_content_selector::record) return;
	do_check = false;
	for(auto& f:obj.mem_funcs) convert(f);
	do_check = true;
	for(auto& c:obj.constructors) for(auto cp:c.func_params) convert(cp.param_type);
}

void modegen::helpers::type_converter::convert(modegen::type& t) const
{
	if(t.modificator == "list") t.modificator = "std::vector";
	else if(t.modificator == "map") t.modificator = "std::map";
	else if(t.modificator == "optional") t.modificator = "std::optional";

	auto npos = type_maps.find(t.name);
	if(npos!=type_maps.end()) t.name = npos->second;
}
