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
	//data = data | tcvt ;

	cppjson::value jsoned = data | tcvt | interface::to_json();

	auto incs = tcvt.includes();
	for(std::size_t i=0;i<incs.size();++i) jsoned["incs"][i]["n"] = incs[i];

	return jsoned;
}

