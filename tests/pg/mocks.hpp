#pragma once
/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <turtle/mock.hpp>

#include "pg/provider.hpp"
#include "pg/part_descriptor.hpp"
#include "pg/output_descriptor.hpp"
#include "parser/interface/loader.hpp"

namespace pgmocks {
MOCK_BASE_CLASS( mock_provider, modegen::pg::provider)
{
	MOCK_METHOD( create_part, 1 )
	MOCK_METHOD( create_output, 2 )
	MOCK_METHOD( input, 0 )
};

MOCK_BASE_CLASS( mock_part, modegen::pg::part_descriptor )
{
	MOCK_METHOD( lang, 0 )
	MOCK_METHOD( name, 0 )
	MOCK_METHOD( outputs, 0 )
	MOCK_METHOD( build_outputs, 2 )
};

MOCK_BASE_CLASS( iloader, modegen::parser::interface::loader )
{
	void load(std::istream& /*input*/, std::string /*fn*/) override {}
	void load(FS::path /*file*/) override {}

	MOCK_METHOD( result, 0 )
	MOCK_METHOD( finish_loads, 0 )
};

MOCK_BASE_CLASS( part_output, modegen::pg::output_descriptor )
{
	MOCK_METHOD( data, 0 );
	MOCK_METHOD( file, 0 );
};

template<typename Vec, typename... Args>
auto make_vector(Vec&& obj, Args&&... objs)
{
	using first_type = std::decay_t<Vec>;
	return std::vector<first_type>{
		std::forward<Vec>(obj),
		std::forward<Args>(objs)...
	};
}

} // namespace pgmocks
