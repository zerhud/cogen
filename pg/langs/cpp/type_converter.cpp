/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "type_converter.hpp"

#include "parser/interface/helpers.hpp"
#include "exceptions.hpp"

using namespace std::literals;
namespace mi = modegen::parser::interface;
namespace mcpp = modegen::pg::cpp;

std::map<std::string,std::string> mcpp::type_converter::type_maps =
{
      {"i8"s,"std::int8_t"s}
    , {"i16"s,"std::int16_t"s}
    , {"i32"s,"std::int32_t"s}
    , {"i64"s,"std::int64_t"s}
    , {"f32"s,"float"s}
    , {"f64"s,"double"s}
    , {"string"s,"std::string"s}
    , {"date"s,"std::chrono::system_clock::time_point"s}
    , {"binary"s,"std::vector<std::byte>"s}
    , {"list"s,"std::vector"s}
    , {"map"s,"std::map"s}
    , {"optional"s,"std::optional"s}
    , {"void"s,"void"s}
};

std::map<std::string,std::string> mcpp::type_converter::incs_maps =
{
      {"i8"s,"cstdint"s}
    , {"i16"s,"cstdint"s}
    , {"i32"s,"cstdint"s}
    , {"i64"s,"cstdint"s}
    , {"string"s,"string"s}
    , {"date"s,"chrono"s}
    , {"binary"s,"vector"s}
    , {"list"s,"vector"s}
    , {"map"s,"map"s}
    , {"optional"s,"optional"s}
};

mcpp::type_converter::type_converter()
{
}

std::vector<mi::module>& mcpp::type_converter::operator()(std::vector<mi::module>& mods)
{
	using namespace mi;

	auto v = [this](auto& mc) { convert(mc); };
	for(auto& mod:mods) {
		cur_mod = &mod;
		for(auto& mc:mod.content) std::visit(v, mc);

		std::sort(cur_mod->imports.begin(),cur_mod->imports.end(),[](using_directive& left, using_directive& right){return left.mod_name < right.mod_name;});
		auto upos = std::unique(cur_mod->imports.begin(),cur_mod->imports.end(),
		                        [](using_directive& left, using_directive& right){return left.mod_name==right.mod_name;});
		cur_mod->imports.erase(upos,cur_mod->imports.end());

		for(auto& i:cur_mod->imports) if(i.is_from_system()) total_incs.emplace_back(i.mod_name);
	}

	std::sort(total_incs.begin(),total_incs.end());
	total_incs.erase(std::unique(total_incs.begin(),total_incs.end()), total_incs.end());

	cur_mod = nullptr;

	return mods;
}

std::vector<std::string> mcpp::type_converter::includes() const
{
	return total_incs;
}

void mcpp::type_converter::add_to_imports(std::string mn, bool sys, std::optional<parser::interface::export_info> ei)
{
	using namespace mi;

	auto pos = std::find(cur_mod->imports.begin(), cur_mod->imports.end(), mn);
	if(pos==cur_mod->imports.end()) {
		using_directive item{mn,sys,{}};
		if(ei) item.dest_items.emplace_back(std::move(*ei));
		cur_mod->imports.emplace_back(std::move(item));
	}
	else if(ei) {
		auto ipos = std::find(pos->dest_items.begin(),pos->dest_items.end(), ei->name);
		if(ipos == pos->dest_items.end()) pos->dest_items.emplace_back(std::move(*ei));
	}
}

void mcpp::type_converter::convert(mi::function& obj)
{
	using namespace mi;

	export_info ei{obj.name, module_content_selector::function};
	convert(obj.return_type, ei);
	for(auto& p:obj.func_params) convert(p.param_type, ei);
}

void mcpp::type_converter::convert(mi::record& obj)
{
	using namespace mi;

	assert(cur_mod);

	export_info ei{obj.name, module_content_selector::record};
	add_to_imports("memory"s, true, ei);
	for(auto& m:obj.members) convert(m.param_type, ei);

	define_type(obj.name, mi::module_content_selector::record);
}

void mcpp::type_converter::convert(mi::enumeration& obj)
{
	using namespace mi;

	add_to_imports("string"s, true, export_info{obj.name, module_content_selector::enumeration});
	define_type(obj.name, module_content_selector::enumeration);
}

void mcpp::type_converter::convert(mi::interface& obj)
{
	using namespace mi;

	assert(cur_mod);

	export_info ei{obj.name, module_content_selector::interface};
	add_to_imports("memory"s, true, ei);
	for(auto& f:obj.mem_funcs) convert(f);
	for(auto& c:obj.constructors) {
		for(auto& cp:c.func_params) {
			convert(cp.param_type, ei);
		}
	}

	define_type(obj.name, mi::module_content_selector::interface);
}

void mcpp::type_converter::convert(mi::type& t, const mi::export_info& ei)
{
	assert(cur_mod);

	auto ipos = incs_maps.find(t.name);
	if(ipos!=incs_maps.end()) add_to_imports(ipos->second, true, ei);

	auto npos = type_maps.find(t.name);
	if(npos!=type_maps.end()) t.name = npos->second;
	else {
		if(!t.sub_types.empty()) throw errors::error("cpp_type_cvt: defined type cannot have parameters");
		defined_types.emplace_back(type_info{t});
	}

	for(auto& s:t.sub_types) convert(s, ei);
}

void mcpp::type_converter::define_type(std::string_view name, mi::module_content_selector from)
{
	for(auto& t:defined_types) if(t.type.name == name) t.points = from;
}
