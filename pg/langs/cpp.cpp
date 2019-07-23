/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "cpp.hpp"
#include "part_descriptor.hpp"
#include "part_algos/module.hpp"
#include "part_algos/interface/to_json.hpp"
//#include "cpp/type_converter.hpp"
#include "exceptions.hpp"

namespace mpg = modegen::pg;
namespace mgo = modegen::pg::outputs;
namespace mpi = modegen::parser::interface;
namespace mai = modegen::generation::interface;

mgo::cpp::cpp(provider_const_ptr p, FS::path o, const part_descriptor& part)
	: out_file_(std::move(o))
	, imngs_(part.input_managers())
{
}

mpg::output_lang mgo::cpp::lang() const
{
	return output_lang::cpp;
}

void mgo::cpp::override_setts(boost::property_tree::ptree s)
{
}

nlohmann::json mgo::cpp::data() const
{
	nlohmann::json ret;

	std::shared_ptr<mpg::palgos::module_algos> malg;
	for(auto&i:imngs_) if(malg=std::dynamic_pointer_cast<mpg::palgos::module_algos>(i);malg) break;
	assert(malg);

	mai::to_json jsoner;

	auto mods = malg->mods();
	ret = jsoner(mods);

	return ret;
}

FS::path mgo::cpp::file() const {
	return out_file_;
}

