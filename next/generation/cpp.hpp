#pragma once

#include "file_data.hpp"

namespace modegen::generation {

class cpp_generator : public file_data {
public:
	cppjson::value jsoned_data(parser::loader_ptr data_loader, options_view opts) const override ;
};

} // namespace modegen::generation
