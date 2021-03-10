#include "external_module_1_1/module.hpp"

namespace em = external_module_1_1;

std::pmr::string em::gen_str(em::external_enum val)
{
	if(val == external_enum::ex_one) return "gen_one";
	if(val == external_enum::ex_two) return "gen_two";
	return "ups";
}

void em::fill_data(em::varius_data_ptr d, std::optional<std::pmr::string> msg)
{
	d->message = msg;
	d->int_data = 10;
}
