#include "cpp.hpp"

#include "errors.h"
#include "interface/to_json.hpp"
#include "cpp/type_converter.hpp"
#include "parser/interface/loader.hpp"

using namespace std::literals;
namespace mg = modegen::generation;

cppjson::value mg::cpp_generator::jsoned_data(parser::loader_ptr data_loader, options_view opts) const
{
	using namespace modegen::generation::interface;

	auto* ildr = dynamic_cast<parser::interface::loader*>(data_loader.get());
	if(!ildr) throw errors::gen_error("cpp"s, "cannot generate cpp not from interface declaration or with wrong loader"s);

	cpp::type_converter tcvt;

	auto data = ildr->result();

	cppjson::value jsoned = data | tcvt | interface::to_json();

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

