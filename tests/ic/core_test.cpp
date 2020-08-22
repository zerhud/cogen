/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ic_core

#include <boost/test/unit_test.hpp>

#include "mocks.hp"

using namespace std::literals;


BOOST_AUTO_TEST_SUITE(input_configurator)
BOOST_AUTO_TEST_SUITE(core)

struct core_fixture {
	mic::core core;
	std::shared_ptr<icmocks::input> in = std::make_shared<icmocks::input>();
	std::shared_ptr<icmocks::output> out = std::make_shared<icmocks::output>();
	std::shared_ptr<icmocks::provider> prov = std::make_shared<icmocks::provider>();
	std::shared_ptr<icmocks::configuration> config = std::make_shared<icmocks::configuration>();
};

BOOST_FIXTURE_TEST_CASE(simple, core_fixture)
{
	//MOCK_EXPECT();

	BOOST_CHECK_THROW(core.gen(nullptr, out, config), std::exception);
	BOOST_CHECK_THROW(core.gen(in, nullptr, config), std::exception);
	BOOST_CHECK_THROW(core.gen(in, out, nullptr), std::exception);
}

BOOST_FIXTURE_TEST_CASE(all_to_one_1, core_fixture)
{
	MOCK_EXPECT(in->to_json).returns("{\"test\":true}"_json);
	MOCK_EXPECT(config->outputs).returns(
	            std::vector<mic::configuration::output_info>{
	                {"test1"s,"t1"s},{"test2"s,"t2"s}});
	auto mut1 = std::make_shared<icmocks::transformation>();
	auto mut2 = std::make_shared<icmocks::transformation>();
	MOCK_EXPECT(config->mutators).returns(
	            std::vector<std::shared_ptr<mic::transformation>>{mut1, mut2});
	MOCK_EXPECT(mut1->do_trans)
	        .once().calls([this](mic::input& i){BOOST_TEST(&i==in.get());});
	MOCK_EXPECT(mut2->do_trans)
	        .once().calls([this](mic::input& i){BOOST_TEST(&i==in.get());});

	mock::sequence adding;
	MOCK_EXPECT(out->add).once().in(adding).calls(
	            [](std::string n, std::string t, nlohmann::json d){
		BOOST_TEST(t=="t1"s);
		BOOST_TEST(n=="test1"s);
		BOOST_TEST(d=="{\"test\":true}"_json );
	});
	MOCK_EXPECT(out->add).once().in(adding).calls(
	            [](std::string n, std::string t, nlohmann::json d){
		BOOST_TEST(t=="t2"s);
		BOOST_TEST(n=="test2"s);
		BOOST_TEST(d=="{\"test\":true}"_json );
	});

	core.gen(in, out, config);
}

BOOST_FIXTURE_TEST_CASE(transformations, core_fixture)
{
	;
}

BOOST_AUTO_TEST_SUITE_END() // core
BOOST_AUTO_TEST_SUITE_END() // input_configurator
