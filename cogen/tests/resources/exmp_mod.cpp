#include "mod_1_0/module.hpp"
#include "mod_1_0/pybind.hpp"

namespace mod = mod_1_0;

class echo_srv : public mod::echo_service {
	mod::echo_callback_ptr cb;
	void generate(mod::echo_mesage_ptr msg) override
	{
		if(!cb) return;
		if(msg->msg) cb->on_message(*msg->msg);
		else cb->on_message("no value");
	}
public:
	echo_srv(mod::echo_callback_ptr c) : cb(std::move(c)) {}
};

mod::echo_service_ptr mod::create_echo_service(std::int64_t start_form, mod::echo_callback_ptr cb)
{
	return std::make_shared<echo_srv>(cb);
}
