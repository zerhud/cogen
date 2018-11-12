#pragma once

#include <iostream>
#include <functional>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/repository/include/qi_kwd.hpp>
#include <boost/spirit/repository/include/qi_distinct.hpp>
#include <boost/phoenix.hpp>
#include <boost/bind.hpp>

#include "modegen.hpp"

namespace qi = boost::spirit::qi;
namespace pl = std::placeholders;

namespace modegen {
template<typename Iterator>
struct grammar : boost::spirit::qi::grammar<Iterator, std::vector<module>(), boost::spirit::qi::ascii::space_type>
{
	grammar() : grammar::base_type(modules_rule, "modegen")
	{
		using qi::eps;
		using qi::lit;
		using qi::_val;
		using qi::_1;
		using qi::ascii::char_;
		using boost::spirit::repository::qi::kwd;
		using boost::spirit::repository::qi::distinct;
		using qi::ascii::blank;
		using qi::ascii::alpha;
		using qi::ascii::space;

		using boost::phoenix::at_c;
		using boost::phoenix::push_back;

		var_name.name("var_name");
		var_name = qi::lexeme[qi::ascii::alpha[_val+=_1] >> *(qi::ascii::alpha[_val+=_1]|qi::ascii::digit[_val+=_1]|lit('_')[_val+='_'])];

		mutable_mod.name("mutable_mod");
		mutable_mod = qi::lexeme[(lit("const")[_val=false] | lit("mutable")[_val=true])];

		type_rule.name("type_definition");
		type_rule = var_name[at_c<0>(_val)=_1] | (var_name[at_c<1>(_val)=_1] > lit('<') > var_name[at_c<0>(_val)=_1] > lit('>'));

		function_param_rule.name("function_param");
		function_param_rule %= type_rule > var_name;

		module_rule.name("module_rule");
		module_rule = qi::lexeme[lit("module") > +blank > var_name[at_c<0>(_val)=_1]] > ':' > *(
		               (function_rule)[push_back(at_c<1>(_val),_1)] > lit(';')
		               );

		modules_rule.name("modules_rule");
		modules_rule %= +module_rule;

		method_rule.name("method_rule");
		function_rule.name("function_rule");
		function_rule %= type_rule >> var_name >> lit('(') >> (-(function_param_rule % ',')) > lit(')');
		method_rule %= type_rule >> var_name >> lit('(') >> (-(function_param_rule % ',')) >> lit(')') >> mutable_mod;

		qi::on_error<qi::fail>
		(
		    modules_rule
		  , std::cout
		        << boost::phoenix::val("Error! Expecting ")
		        << qi::_4                               // what failed?
		        << boost::phoenix::val(" here: \"")
		        << boost::phoenix::construct<std::string>(qi::_3, qi::_2)   // iterators to error-pos, end
		        << boost::phoenix::val("\"")
		        << std::endl
		);
	}

	qi::rule<Iterator, bool()> mutable_mod;
	qi::rule<Iterator, std::string()> var_name;
	qi::rule<Iterator, type(), boost::spirit::qi::ascii::space_type> type_rule;
	qi::rule<Iterator, function(), boost::spirit::qi::ascii::space_type> function_rule;
	qi::rule<Iterator, function(), boost::spirit::qi::ascii::space_type> method_rule;
	qi::rule<Iterator, func_param(), boost::spirit::qi::ascii::space_type> function_param_rule;
	qi::rule<Iterator, module(), boost::spirit::qi::ascii::space_type> module_rule;
	qi::rule<Iterator, std::vector<module>(), boost::spirit::qi::ascii::space_type> modules_rule;
};
} // namespace modegen
