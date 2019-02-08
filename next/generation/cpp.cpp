#include "cpp.hpp"

#include "errors.h"
#include "interface/to_json.hpp"
#include "parser/interface/loader.hpp"

using namespace std::literals;
namespace mg = modegen::generation;

cppjson::value mg::cpp_generator::jsoned_data(parser::loader_ptr data_loader, options_view opts) const
{
	auto* ildr = dynamic_cast<parser::interface::loader*>(data_loader.get());
	if(!ildr) throw errors::gen_error("cpp"s, "cannot generate cpp not from interface declaration or with wrong loader"s);

	auto data = ildr->result();
	return interface::to_json()(data);
}

