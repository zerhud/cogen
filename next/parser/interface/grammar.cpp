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

namespace modegen::parser::interface {
template<typename Iterator>
struct grammar : boost::spirit::qi::grammar<Iterator, parsed_file(), boost::spirit::qi::ascii::space_type>
{
	grammar() : grammar::base_type(file_rule, "modegen")
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
		using boost::phoenix::new_;
		using boost::phoenix::construct;

		quoted_string.name("quoted_string");
		quoted_string =
		        ('"' >> *(char_[_val+=_1] - lit('"')) >> '"') |
		        ("'" >> *(char_[_val+=_1] - lit("'")) >> "'")
		        ;

		var_name.name("var_name");
		var_name = +(char_("a-zA-Z0-9:_")[_val+=_1]);

		mutable_mod.name("mutable_mod");
		mutable_mod = qi::lexeme[(lit("const")[_val=false] | lit("mutable")[_val=true])];

		type_rule.name("type_definition");
		//type_rule = (var_name[push_back(at_c<0>(_val),_1)] >> !(*space >> lit('<')))
		          //| (var_name[at_c<1>(_val)=_1] >> *space >> lit('<') >> *space >> (var_name[push_back(at_c<0>(_val),_1)]%',') >> *space >> lit('>'));
		type_rule = var_name[at_c<0>(_val)=_1]
		          >> -(lit('<')
		          >> (type_rule[push_back(at_c<1>(_val),(new_<type>(_1)))]%',')
		          >> lit('>'));

		using_rule.name("using_rule");
		using_rule = lit("using") > +blank > var_name[at_c<0>(_val)=_1];

		version_rule.name("version_rule");
		version_rule = lit('v') >> qi::uint_ >> lit('.') >> qi::uint_;

		documentation_rule.name("documentation_rule");
		documentation_rule %= *blank >> lit("#") >> *(char_[at_c<0>(_val)+=_1] - qi::eol) >> qi::eol;

		deprication_rule.name("depricated_rule");
		deprication_rule %= lit("@depricated") > version_rule[at_c<0>(_val)=_1] > -('(' > quoted_string[at_c<1>(_val)=_1] > ')');

		meta_params_rule =
		         *(
		              (+(documentation_rule[push_back(_val,_1)]))
		            | deprication_rule[push_back(_val,_1)]
		            | ('@' > version_rule[push_back(_val,_1)])
		          );

		function_param_rule.name("function_param");
		function_param_rule %= type_rule > var_name;

		file_rule.name("system_rule");
		file_rule =
		  *((lit("include") >> quoted_string[push_back(at_c<1>(_val),_1)]) | module_rule[push_back(at_c<0>(_val),_1)] )
		         ;

		module_rule.name("module_rule"); // module mod_name v123:
		module_rule = meta_params_rule[at_c<2>(_val)=_1] >>
		              qi::lexeme[lit("module")
		                          > +blank > var_name[at_c<0>(_val)=_1]
		                          > +blank > version_rule[push_back(at_c<2>(_val),_1)]] > ':' > *(
		                ((function_rule)[push_back(at_c<1>(_val),_1)] > lit(';'))
		              | (enum_rule[push_back(at_c<1>(_val),_1)])
		              | (record_rule[push_back(at_c<1>(_val),_1)])
		              | (interface_rule[push_back(at_c<1>(_val),_1)])
		              | (using_rule[push_back(at_c<3>(_val),_1)])
		               );

		function_rule.name("function_rule");
		function_rule = meta_params_rule[at_c<3>(_val)=_1]
		              >> -qi::lexeme[lit("static")[at_c<5>(_val)=true] >> space]
		              >> type_rule[at_c<0>(_val)=_1]
		              >> var_name[at_c<1>(_val)=_1]
		              >> lit('(') >> (-(function_param_rule[push_back(at_c<2>(_val),_1)] % ',')) > lit(')')
		              >> -mutable_mod[at_c<4>(_val)=_1]
		              ;

		constructor_rule.name("constructor_rule");
		constructor_rule =  meta_params_rule[at_c<1>(_val)=_1] >> lit("constructor")
		                 >> lit('(') >> (-(function_param_rule[push_back(at_c<0>(_val),_1)] % ',')) > lit(')')
		  ;


		enum_element_rule.name("enum_elemen");
		enum_element_rule = var_name[at_c<0>(_val)=_1] >> -(lit("=>") >> quoted_string[at_c<1>(_val)=_1]);

		enum_rule.name("enum_rule");
		enum_rule = meta_params_rule[at_c<2>(_val)=_1]
		          >> qi::lexeme[lit("enum") >> space]
		          >> -qi::lexeme[lit("+flags")[at_c<4>(_val)=true] >> space]
		          >> -qi::lexeme[lit("+auto_io")[at_c<3>(_val)=true] >> space]
		          >> var_name[at_c<0>(_val)=_1]
		          >> lit('{')
		          >> *(enum_element_rule[push_back(at_c<1>(_val),_1)])
		          >> lit('}')
		           ;

		record_item_rule.name("record_item");
		record_item_rule = meta_params_rule[at_c<0>(_val)=_1]
		            >> type_rule[at_c<1>(_val)=_1]
		            >> var_name[at_c<2>(_val)=_1]
		  ;

		record_rule.name("record_rule");
		record_rule = meta_params_rule[at_c<2>(_val)=_1]
		            >> qi::lexeme[lit("record") >> +space >> var_name[at_c<0>(_val)=_1]]
		            >> lit('{')
		            >> *(record_item_rule[push_back(at_c<1>(_val),_1)] >> lit(';'))
		            >> lit('}')
		  ;

		interface_rule.name("interface_rule");
		interface_rule = meta_params_rule[at_c<3>(_val)=_1]
		                         >> qi::lexeme[lit("interface") >> +space >> -lit("+i")[at_c<4>(_val)=true] >> var_name[at_c<0>(_val)=_1]]
		                         >> lit('{')
		                         >> *((function_rule[push_back(at_c<1>(_val),_1)] | constructor_rule[push_back(at_c<2>(_val),_1)]) >> lit(';'))
		                         >> lit('}')
		  ;
	}

	qi::rule<Iterator, bool()> mutable_mod;
	qi::rule<Iterator, std::string()> var_name;
	qi::rule<Iterator, std::string()> quoted_string;
	qi::rule<Iterator, using_directive> using_rule;
	qi::rule<Iterator, type(), boost::spirit::qi::ascii::space_type> type_rule;
	qi::rule<Iterator, function(), boost::spirit::qi::ascii::space_type> function_rule;
	qi::rule<Iterator, constructor_fnc(), boost::spirit::qi::ascii::space_type> constructor_rule;
	qi::rule<Iterator, func_param(), boost::spirit::qi::ascii::space_type> function_param_rule;
	qi::rule<Iterator, enum_element(), boost::spirit::qi::ascii::space_type> enum_element_rule;
	qi::rule<Iterator, enumeration(), boost::spirit::qi::ascii::space_type> enum_rule;
	qi::rule<Iterator, record_item(), boost::spirit::qi::ascii::space_type> record_item_rule;
	qi::rule<Iterator, record(), boost::spirit::qi::ascii::space_type> record_rule;
	qi::rule<Iterator, interface(), boost::spirit::qi::ascii::space_type> interface_rule;
	qi::rule<Iterator, module(), boost::spirit::qi::ascii::space_type> module_rule;
	qi::rule<Iterator, meta_parameters::version()> version_rule;
	qi::rule<Iterator, meta_parameters::documentation()> documentation_rule;
	qi::rule<Iterator, meta_parameters::deprication(), boost::spirit::qi::ascii::space_type> deprication_rule;
	qi::rule<Iterator, meta_parameters::parameter_set, boost::spirit::qi::ascii::space_type> meta_params_rule;
	qi::rule<Iterator, parsed_file(), boost::spirit::qi::ascii::space_type> file_rule;
};

namespace error_hander {
struct printer
{
	typedef boost::spirit::utf8_string string;

	mutable std::stringstream result;

	void element(string const& tag, string const& value, int depth) const
	{
		for (int i = 0; i < (depth*4); ++i) // indent to depth
			result << ' ';

		result << "tag: " << tag;
		if (value != "") result << ", value: " << value;
		result << std::endl;
	}
};

std::string print_info(boost::spirit::info const& what)
{
	using boost::spirit::basic_info_walker;

	printer pr;
	basic_info_walker<printer> walker(pr, what.tag, 0);
	boost::apply_visitor(walker, what.value);

	return pr.result.str();
}

struct inner_error : std::runtime_error {
	//std::string w;
	//inner_error(std::string s) : w(std::move(s)) {}
	//const char* what() const noexcept override {return w.c_str();}
	inner_error(std::string s) : std::runtime_error(s) {}
};
} // namespace error_handler
} // namespace modegen::parser::interface


modegen::parser::interface::parsed_file modegen::parser::interface::parse(std::string_view pdata)
{
	auto begin = pdata.begin();
	auto end = pdata.end();

	modegen::parser::interface::parsed_file result;

	bool r=false;
	try {
		grammar<decltype(begin)> mg_parser;
		r = boost::spirit::qi::phrase_parse(begin,end, mg_parser, boost::spirit::ascii::space, result);
	}
	catch(boost::spirit::qi::expectation_failure<const char*>& ex) {
		auto result = error_hander::print_info(ex.what_);
		result += "\n\nnot parsed part:\n" + std::string(ex.first,ex.last);
		throw error_hander::inner_error(std::move(result));
	}

	if(!r) throw std::runtime_error("cannot parse");
	return result;
}
