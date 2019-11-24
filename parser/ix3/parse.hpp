/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include <iostream>
#include <string_view>
#include <type_traits>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/support/utf8.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>

namespace ix3::text {

	namespace x3 = boost::spirit::x3;

	struct parser_env {};

	using iterator_type = std::string_view::const_iterator;
	using error_handler_type = x3::error_handler<iterator_type>;
	using context_type = x3::context
		<
		 parser_env,parser_env
		,x3::context
		 <
		  x3::error_handler_tag,std::reference_wrapper<error_handler_type>
		 ,x3::phrase_parse_context<decltype(x3::space)>::type
		 >
		>;

	template<typename Parser, typename Env, typename ErrHndl>
	auto make_grammar(const Parser& parser, Env&& env, ErrHndl& eh)
	{
		return
			x3::with<x3::error_handler_tag,std::reference_wrapper<ErrHndl>>(std::ref(eh))
			[
				boost::spirit::x3::with<Env,Env>(std::move(env))
				[
					parser
				]
			]
			;
	}

	template<typename Id, typename Attribute, typename Iterator>
	Attribute parse(boost::spirit::x3::rule<Id, Attribute> rule, Iterator begin, Iterator end, parser_env&& env=parser_env{})
	{
		Attribute result;
		//error_handler_type eh(begin, end, std::cerr);
		x3::error_handler<Iterator> eh(begin, end, std::cerr);
		bool success = boost::spirit::x3::phrase_parse(begin, end, make_grammar(rule, std::move(env), eh), boost::spirit::x3::space, result);

		if(!success) throw std::runtime_error("cannot parse");
		if(begin!=end) throw std::runtime_error("parse not finished");

		return result;
	}

	template<typename Id, typename Attribute>
	Attribute parse(boost::spirit::x3::rule<Id, Attribute> rule, std::string_view data, parser_env&& env=parser_env{})
	{
		//auto end = boost::u8_to_u32_iterator(data.end());
		//auto begin = boost::u8_to_u32_iterator(data.begin());
		auto end = data.end();
		auto begin = data.begin();

		return parse(rule, begin, end, std::move(env));
	}

} // namespace ix3::text
