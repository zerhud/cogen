/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/support/utf8.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>
#include "env.hpp"

namespace ix3::text {

	namespace x3 = boost::spirit::x3;

	using iterator_type = std::string_view::const_iterator;
	using error_handler_type = x3::error_handler<iterator_type>;
	using context_type = x3::context
	    <
	     parser_env_tag,std::reference_wrapper<const parser_env>
	    ,x3::context
	     <
	      x3::error_handler_tag,std::reference_wrapper<error_handler_type>
	     ,x3::phrase_parse_context<decltype(x3::space)>::type
	     >
	    >;

	template<typename Parser, typename Env, typename ErrHndl>
	auto make_grammar(const Parser& parser, const Env& env, ErrHndl& eh)
	{
		return
			x3::with<x3::error_handler_tag>(std::ref(eh))
			[
				boost::spirit::x3::with<parser_env_tag>(std::cref(env))
				[
					parser
				]
			]
			;
	}

	template<typename Id, typename Attribute, typename Iterator>
	Attribute parse(
	        boost::spirit::x3::rule<Id, Attribute> rule,
	        Iterator begin, Iterator end,
	        const parser_env& env=cerr_throw_env{""})
	{
		Attribute result;
		x3::error_handler eh(begin, end, env.out(), env.file_name());
		bool success = boost::spirit::x3::phrase_parse(
		            begin, end,
		            make_grammar(rule, env, eh),
		            boost::spirit::x3::space,
		            result);

		if(!success) env.on_error("cannot parse");
		if(begin!=end) env.on_error("parse not finished");

		return result;
	}

	template<typename Id, typename Attribute>
	Attribute parse(
	        boost::spirit::x3::rule<Id, Attribute> rule,
	        std::string_view data,
	        const parser_env& env=cerr_throw_env{""})
	{
		//auto end = boost::u8_to_u32_iterator(data.end());
		//auto begin = boost::u8_to_u32_iterator(data.begin());
		auto end = data.end();
		auto begin = data.begin();

		return parse(rule, begin, end, env);
	}

} // namespace ix3::text
