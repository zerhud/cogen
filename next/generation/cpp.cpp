#include "cpp.hpp"

#include "errors.h"
#include "interface/naming.hpp"
#include "interface/to_json.hpp"
#include "interface/filter.hpp"
#include "cpp/type_converter.hpp"
#include "parser/interface/loader.hpp"
#include "parser/interface/helpers.hpp"

using namespace std::literals;
namespace mg = modegen::generation;
namespace mi = modegen::parser::interface;

namespace modegen::generation {
struct json_extra_info : interface::to_json_aspect {
	void as_object(cppjson::value& jval, const mi::module& obj) override
	{
		jval["namespace"] = obj.name + "_v"s + get_version(obj).value("_"sv) ;
	}
};
} // modegen::generation

cppjson::value mg::cpp_generator::jsoned_data(parser::loader_ptr data_loader, options_view opts) const
{
	using namespace modegen::generation::interface;
	using modegen::generation::interface::operator |;

	auto* ildr = dynamic_cast<parser::interface::loader*>(data_loader.get());
	if(!ildr) throw errors::gen_error("cpp"s, "cannot generate cpp not from interface declaration or with wrong loader"s);

	cpp::type_converter tcvt;

	auto data = ildr->result();
	if(data.empty()) throw errors::gen_error("cpp"s, "cannot work with empty interface");

	filter::request freq;
	freq.mod_name = opts.part_data().get("filter.mod", ""s);
	freq.cnt_name = opts.part_data().get("filter.cnt", ""s);
	freq.sel = parser::interface::from_string(opts.part_data().get("filter.sel", ""s));

	cppjson::value jsoned =
	          data
	        | tcvt
	        | filter(freq)
	        | naming(opts.part_data().get("naming",""s))
	        | interface::to_json(std::make_unique<json_extra_info>())
	        ;

	auto incs = includes(tcvt.includes(), opts);
	for(std::size_t i=0;i<incs.size();++i) {
		jsoned["incs"][i]["n"] = incs[i].name;
		jsoned["incs"][i]["sys"] = incs[i].sys;
	}

	return jsoned;
}

std::vector<mg::cpp_generator::inc_info> mg::cpp_generator::includes(const std::vector<std::string> sys, mg::options_view& opts) const
{
	std::vector<inc_info> ret;
	ret.reserve(sys.size());

	for(auto& s:sys) ret.emplace_back(std::move(s), true);
	for(auto& i:opts.part_data()) {
		if(i.first == "inc_sys"sv) {
			ret.emplace_back(i.second.get_value<std::string>(), true);
		}
	}
	for(auto& i:opts.part_data()) {
		if(i.first == "inc_part"sv) {
			ret.emplace_back(solve_part_include(i.second.get_value<std::string>(), opts), false);
		}
	}
	for(auto& i:opts.part_data()) {
		if(i.first == "inc_local"sv) {
			ret.emplace_back(i.second.get_value<std::string>(), false);
		}
	}

	return ret;
}

std::string mg::cpp_generator::solve_part_include(const std::string& part, mg::options_view& opts) const
{
	return opts.part_data(part).get<std::string>("output");
}

