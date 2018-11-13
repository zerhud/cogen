#include "grammar.hpp"


#include <iostream>
#include <functional>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/repository/include/qi_kwd.hpp>
#include <boost/spirit/repository/include/qi_distinct.hpp>
#include <boost/phoenix.hpp>
#include <boost/bind.hpp>

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

		quoted_string.name("quoted_string");
		quoted_string = '"' >> *(char_[_val+=_1] - lit('"')) >> '"';

		var_name.name("var_name");
		var_name = qi::lexeme[qi::ascii::alpha[_val+=_1] >> *(qi::ascii::alpha[_val+=_1]|qi::ascii::digit[_val+=_1]|lit('_')[_val+='_'])];

		mutable_mod.name("mutable_mod");
		mutable_mod = qi::lexeme[(lit("const")[_val=false] | lit("mutable")[_val=true])];

		type_rule.name("type_definition");
		type_rule = var_name[at_c<0>(_val)=_1] | (var_name[at_c<1>(_val)=_1] > lit('<') > var_name[at_c<0>(_val)=_1] > lit('>'));

		version_rule.name("version_rule");
		version_rule = lit('v') >> qi::uint_;

		documentation_rule.name("documentation_rule");
		documentation_rule %= *blank >> lit("#") >> *(char_[at_c<0>(_val)+=_1] - qi::eol) >> qi::eol;

		deprication_rule.name("depricated_rule");
		deprication_rule %= lit("@depricated") > -('(' > quoted_string > ')') > space;

		meta_params_rule =
		         *(
		              (+(documentation_rule[push_back(_val,_1)]))
		            | deprication_rule[push_back(_val,_1)]
		            | ('@' > version_rule[push_back(_val,_1)])
		          );

		function_param_rule.name("function_param");
		function_param_rule %= type_rule > var_name;

		module_rule.name("module_rule"); // module mod_name v123:
		module_rule = meta_params_rule[at_c<2>(_val)=_1] >>
		              qi::lexeme[lit("module")
		                          > +blank > var_name[at_c<0>(_val)=_1]
		                          > +blank > version_rule[push_back(at_c<2>(_val),_1)]] > ':' > *(
		               (function_rule)[push_back(at_c<1>(_val),_1)] > lit(';')
		               );

		modules_rule.name("modules_rule");
		modules_rule %= +module_rule;

		function_rule.name("function_rule");
		function_rule = meta_params_rule[at_c<4>(_val)=_1]
		              >> type_rule[at_c<0>(_val)=_1]
		              >> var_name[at_c<1>(_val)=_1]
		              >> lit('(') >> (-(function_param_rule[push_back(at_c<2>(_val),_1)] % ',')) > lit(')')
		              >> -mutable_mod[at_c<3>(_val)=_1]
		              ;

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
	qi::rule<Iterator, std::string()> quoted_string;
	qi::rule<Iterator, type(), boost::spirit::qi::ascii::space_type> type_rule;
	qi::rule<Iterator, function(), boost::spirit::qi::ascii::space_type> function_rule;
	qi::rule<Iterator, func_param(), boost::spirit::qi::ascii::space_type> function_param_rule;
	qi::rule<Iterator, module(), boost::spirit::qi::ascii::space_type> module_rule;
	qi::rule<Iterator, std::vector<module>(), boost::spirit::qi::ascii::space_type> modules_rule;
	qi::rule<Iterator, modegen::meta_parameters::version()> version_rule;
	qi::rule<Iterator, modegen::meta_parameters::documentation()> documentation_rule;
	qi::rule<Iterator, modegen::meta_parameters::deprication()> deprication_rule;
	qi::rule<Iterator, modegen::meta_parameters::parameter_set> meta_params_rule;
};
} // namespace modegen

std::vector<modegen::module> modegen::parse(std::string_view pdata)
{
	auto begin = pdata.begin();
	auto end = pdata.end();

	std::vector<modegen::module> result;

	modegen::grammar<decltype(begin)> mg_parser;
	bool r = boost::spirit::qi::phrase_parse(begin,end, mg_parser, boost::spirit::ascii::space, result);

	if(!r) throw std::runtime_error("cannot parse");
	return result;
}
