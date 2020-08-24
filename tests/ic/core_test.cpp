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

BOOST_FIXTURE_TEST_CASE(errors, core_fixture)
{
	BOOST_CHECK_THROW(core.gen(nullptr, out, config), std::exception);
	BOOST_CHECK_THROW(core.gen(in, nullptr, config), std::exception);
	BOOST_CHECK_THROW(core.gen(in, out, nullptr), std::exception);
}

BOOST_FIXTURE_TEST_CASE(all_to_one, core_fixture)
{
	auto mut1 = std::make_shared<icmocks::transformation>();
	auto mut2 = std::make_shared<icmocks::transformation>();

	mock::sequence in_seq;
	auto in2 = std::make_shared<icmocks::input>();
	auto in1 = std::make_shared<icmocks::input>();
	MOCK_EXPECT(in->clone).once().in(in_seq).returns(in1);
	MOCK_EXPECT(in->clone).once().in(in_seq).returns(in2);
	MOCK_EXPECT(in1->to_json).returns("{\"test\":1}"_json);
	MOCK_EXPECT(in2->to_json).returns("{\"test\":2}"_json);

	MOCK_EXPECT(config->outputs).returns(
	            std::vector<mic::configuration::output_info>{
	                {"test1"s,"t1"s,{mut1,mut2}},{"test2"s,"t2"s,{}}});
	MOCK_EXPECT(mut1->do_trans)
	        .at_least(1).calls([in1](mic::input& i){BOOST_TEST(&i==in1.get());});
	MOCK_EXPECT(mut2->do_trans)
	        .at_least(1).calls([in1](mic::input& i){BOOST_TEST(&i==in1.get());});

	mock::sequence adding;
	MOCK_EXPECT(out->add).once().in(adding).calls(
	            [](std::string n, std::string t, nlohmann::json d){
		BOOST_TEST(t=="t1"s);
		BOOST_TEST(n=="test1"s);
		BOOST_TEST(d=="{\"test\":1}"_json );
	});
	MOCK_EXPECT(out->add).once().in(adding).calls(
	            [](std::string n, std::string t, nlohmann::json d){
		BOOST_TEST(t=="t2"s);
		BOOST_TEST(n=="test2"s);
		BOOST_TEST(d=="{\"test\":2}"_json );
	});

	core.gen(in, out, config);
}

BOOST_FIXTURE_TEST_CASE(transformations, core_fixture)
{
	;
}

BOOST_AUTO_TEST_SUITE_END() // core
BOOST_AUTO_TEST_SUITE_END() // input_configurator
