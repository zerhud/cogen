/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

#include "generation/common.hpp"
#include "generation/provider.hpp"
#include "generation/file_data.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;
namespace mp = modegen::parser;
namespace fs = FS;

MOCK_BASE_CLASS( provider_mock, modegen::generation::provider )
{
	MOCK_METHOD( json_jinja, 1 )
	MOCK_METHOD( parser, 1 )
	MOCK_METHOD( generator, 1 )
	MOCK_METHOD( resolve_file, 3 )
};

class fake_target : public modegen::parser::loader {
	void load(std::istream& input, std::string fn) override {}
	void load(FS::path file) override {}
	void finish_loads() override {}
};

struct fake_data_gen : modegen::generation::file_data {
	bool gen_data = true;
	cppjson::value jsoned_data(mp::loader_ptr data_loader, mg::options_view opts) const override
	{
		cppjson::value ret;
		if(gen_data) {
			ret["name"] = std::string(opts.part_name());
			ret["test"] = opts.part_data().get<std::string>("test");
		}
		return ret;
	}
};

BOOST_AUTO_TEST_CASE(common_generation)
{
	// tests normal using
	//   1. create provider
	//     1. create all needed loaders
	//     2. set all needed generators (jinja only for now)
	//   2. set options in cg
	//   3. cg checks all loaders are provided (don't start generation before trust we have all info)
	//   4. for each resulting file
	//     1. cg calls file_data with it's loader for get data
	//     2. fd applies settings (filters, modificators etc)
	//     3. fd converts result to json
	//     4. cg generates resulting file for each item

	bool cout = true;
	auto result_data_checker = [&cout](const mg::tmpl_gen_env& gdata) {
			if(cout) BOOST_CHECK( !gdata.out_dir() );
			else {
				BOOST_REQUIRE( gdata.out_dir() );
				BOOST_CHECK_EQUAL( *gdata.out_dir(), fs::path("some_dir") / "def.hpp" );
			}

			BOOST_CHECK_EQUAL( gdata.tmpl(), fs::path(u8"resolved/path") / "definitions.hpp.jinja" );
			BOOST_CHECK_EQUAL( gdata.data()["name"], "def"s );
			BOOST_CHECK_EQUAL( gdata.data()["test"], "test_data"s );
			return true;
		};

	auto provider = std::make_shared<provider_mock>();
	MOCK_EXPECT( provider->json_jinja ).exactly(2).with( result_data_checker ) ;
	MOCK_EXPECT( provider->parser ).exactly(2).with( "interface"sv ).returns( std::make_shared<fake_target>() );
	MOCK_EXPECT( provider->generator ).exactly(2).with( "cpp"sv ).returns( std::make_shared<fake_data_gen>() );
	MOCK_EXPECT( provider->resolve_file ).exactly(2).with( "definitions.hpp.jinja"sv, u8"some/path"sv, "cpp"sv ).returns( u8"resolved/path/definitions.hpp.jinja" );

	mg::generator gen(provider, u8"some/path");
	auto& opts = gen.options();
	opts.put("gen.def.target", "cpp") ;
	opts.put("gen.def.parser", "interface") ;
	opts.put("gen.def.output", "def.hpp") ;
	opts.put("gen.def.input", "definitions.hpp.jinja") ;
	opts.put("gen.def.test", "test_data") ;

	// first calls
	gen.generate_stdout("def");

	// second calls
	cout = false;
	gen.generate("some_dir");
}

BOOST_AUTO_TEST_CASE(defaults)
{
	auto provider = std::make_shared<provider_mock>();

	MOCK_EXPECT( provider->json_jinja ).exactly(2);
	MOCK_EXPECT( provider->parser ).exactly(1).with( "interface"sv ).returns( std::make_shared<fake_target>() );
	MOCK_EXPECT( provider->parser ).exactly(1).with( "other_prs"sv ).returns( std::make_shared<fake_target>() );
	MOCK_EXPECT( provider->generator ).exactly(1).with( "cpp"sv ).returns( std::make_shared<fake_data_gen>() );
	MOCK_EXPECT( provider->generator ).exactly(1).with( "other_trg"sv ).returns( std::make_shared<fake_data_gen>() );
	MOCK_EXPECT( provider->resolve_file ).exactly(1).with( "def.hpp.jinja"sv, u8"some/path"sv, "cpp"sv ).returns( u8"resolved/path/def.jinja" );
	MOCK_EXPECT( provider->resolve_file ).exactly(1).with( "other.jinja"sv, u8"some/path"sv, "other_trg"sv ).returns( u8"resolved/path/def.jinja" );

	mg::generator gen(provider, u8"some/path");
	auto& opts = gen.options();
	opts.put("defaults.target", "cpp");
	opts.put("defaults.parser", "interface");
	opts.put("gen.def.output", "def.hpp") ;
	opts.put("gen.def.input", "def.hpp.jinja") ;
	opts.put("gen.def.test", "test_data") ;
	opts.put("gen.other.output", "other.hpp") ;
	opts.put("gen.other.input", "other.jinja") ;
	opts.put("gen.other.target", "other_trg");
	opts.put("gen.other.parser", "other_prs");
	opts.put("gen.other.test", "test_data") ;

	gen.generate("dir");
}

BOOST_AUTO_TEST_CASE(extra_generator_data)
{
	auto provider = std::make_shared<provider_mock>();
	mg::generator gen(provider, u8"some/path");
	auto& opts = gen.options();
	opts.put("defaults.target", "cpp");
	opts.put("defaults.parser", "interface");
	opts.put("defaults.forwards.ex.some.name", "some");
	opts.put("defaults.forwards.ex.some.script", "some script");
	opts.put("gen.def.output", "def.hpp") ;
	opts.put("gen.def.input", "def.hpp.jinja") ;
	opts.put("gen.def.test", "test_data") ;
	opts.put("gen.other.output", "other.hpp") ;
	opts.put("gen.other.input", "other.jinja") ;
	opts.put("gen.other.test", "test_data") ;
	opts.put("gen.other.forwards.ex.other.name", "other");
	opts.put("gen.other.forwards.ex.other.file", "other_file");
	opts.put("gen.other.forwards.before.file", "other_file");
	opts.put("gen.other.forwards.after.script", "other script");

	std::string ex_script_name;
	mg::tmpl_gen_env::script_descriptor ex_script;
	auto result_data_checker = [&ex_script,&ex_script_name](const mg::tmpl_gen_env& gdata) {
		auto efnc = gdata.emb_fnc_list();
		BOOST_CHECK_EQUAL(efnc.size(), 1);
		for(auto& e:efnc) {
			BOOST_CHECK(e.second == ex_script);
			BOOST_CHECK_EQUAL(e.first, ex_script_name);
		}

		if(ex_script_name=="other"s) {
			BOOST_REQUIRE(std::holds_alternative<FS::path>(gdata.exec_before()));
			BOOST_REQUIRE(std::holds_alternative<std::string>(gdata.exec_after()));
			BOOST_CHECK_EQUAL(std::get<FS::path>(gdata.exec_before()), "other_file"s);
			BOOST_CHECK_EQUAL(std::get<std::string>(gdata.exec_after()), "other script"s);
		}

		return true;
	};

	MOCK_EXPECT( provider->json_jinja ).exactly(2).with( result_data_checker ) ;
	MOCK_EXPECT( provider->parser ).exactly(2).with( "interface"sv ).returns( std::make_shared<fake_target>() );
	MOCK_EXPECT( provider->generator ).exactly(2).with( "cpp"sv ).returns( std::make_shared<fake_data_gen>() );
	MOCK_EXPECT( provider->resolve_file ).exactly(1).with( "def.hpp.jinja"sv, u8"some/path"sv, "cpp"sv ).returns( u8"resolved/path/def.jinja" );
	MOCK_EXPECT( provider->resolve_file ).exactly(1).with( "other.jinja"sv, u8"some/path"sv, "cpp"sv ).returns( u8"resolved/path/def.jinja" );
	MOCK_EXPECT( provider->resolve_file ).exactly(1).with( "other_file"sv, u8"some/path"sv, "cpp"sv ).returns( u8"other/path.jinja" );

	ex_script_name = "some"s;
	ex_script = "some script"s;
	BOOST_TEST_CONTEXT("def") gen.generate_stdout("def"sv);
	ex_script_name = "other"s;
	ex_script = FS::path(u8"other/path.jinja"s);
	BOOST_TEST_CONTEXT("other") gen.generate_stdout("other"sv);
}

BOOST_AUTO_TEST_SUITE(options_view)
BOOST_AUTO_TEST_CASE(part_data)
{
	boost::property_tree::ptree pt;
	pt.put("gen.part.test", true);
	pt.put("gen.other.test", true);
	mg::options_view opts(pt, "part"sv);
	BOOST_CHECK_EQUAL(opts.part_data().get<bool>("test"), true);
	BOOST_CHECK_EQUAL(opts.part_data("other"sv).get<bool>("test"), true);
}
BOOST_AUTO_TEST_CASE(all)
{
	boost::property_tree::ptree pt;
	pt.put("gen.part.test", true);
	pt.put("gen.other.test", true);
	pt.put("gen.lala.test", "string");
	mg::options_view opts(pt, "part"sv);

	BOOST_CHECK_EQUAL(&opts.all(), &pt);
}
BOOST_AUTO_TEST_CASE(target_data)
{
	boost::property_tree::ptree pt;
	pt.put("gen.part.test", true);
	mg::options_view opts(pt, "part"sv);

	BOOST_CHECK_NO_THROW( opts.target_data("cpp"sv) );

	pt.put("target.cpp.some", true);
	BOOST_CHECK_EQUAL( opts.target_data("cpp"sv).get<bool>("some"), true );
}
BOOST_AUTO_TEST_SUITE_END() // options_view

BOOST_AUTO_TEST_SUITE(wrong_generation)
BOOST_AUTO_TEST_CASE(no_data)
{
	auto data_gen = std::make_shared<fake_data_gen>();
	data_gen->gen_data = false;

	auto provider = std::make_shared<provider_mock>();
	MOCK_EXPECT( provider->json_jinja ).exactly(0);
	MOCK_EXPECT( provider->parser ).exactly(1).with( "interface"sv ).returns( std::make_shared<fake_target>() );
	MOCK_EXPECT( provider->generator ).exactly(1).with( "cpp"sv ).returns( data_gen );

	mg::generator gen(provider, u8"some/path");
	auto& opts = gen.options();
	opts.put("gen.def.target", "cpp") ;
	opts.put("gen.def.parser", "interface") ;
	opts.put("gen.def.output", "def.hpp") ;
	opts.put("gen.def.input", "definitions.hpp.jinja") ;
	opts.put("gen.def.test", "test_data") ;
	BOOST_CHECK_THROW( gen.generate("some_dir"), modegen::errors::error );
}
BOOST_AUTO_TEST_SUITE_END() // wrong_generation
