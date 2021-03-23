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


} // namespace ix3_mocks
