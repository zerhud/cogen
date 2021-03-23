/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <turtle/mock.hpp>
#include "env.hpp"

namespace ix3_mocks {

MOCK_BASE_CLASS( parser_env, ix3::text::parser_env )
{
	MOCK_METHOD(file_name, 0)
	MOCK_METHOD(on_error, 0, void(), on_err)
	MOCK_METHOD(on_error, 1, void(const std::string&), on_err_msg)
	MOCK_METHOD(out, 0)
	MOCK_METHOD(msg, 1)
};

struct parser_env_info {
	std::string file;
	std::stringstream out;
        std::string err_msg;
        std::string msg, msg_ret;
};

std::shared_ptr<parser_env> create_env(parser_env_info& info)
{
	auto ret = std::make_shared<parser_env>();
	MOCK_EXPECT(ret->file_name).returns(info.file);
	MOCK_EXPECT(ret->out).returns(std::ref(info.out));
        MOCK_EXPECT(ret->on_err);
        if(!info.msg.empty())
            MOCK_EXPECT(ret->msg).with(info.msg).returns(info.msg_ret);
        if(info.err_msg.empty()) MOCK_EXPECT(ret->on_err_msg);
        else MOCK_EXPECT(ret->on_err_msg).with(info.err_msg);
        return ret;
}


} // namespace ix3_mocks
