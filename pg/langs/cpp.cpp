/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "cpp.hpp"

namespace mgo = modegen::pg::outputs;

mgo::cpp::cpp(provider_const_ptr p, FS::path o)
{
}
void mgo::cpp::override_setts(boost::property_tree::ptree s)
{
}

nlohmann::json mgo::cpp::data() const
{
	nlohmann::json ret;
	return ret;
}

FS::path mgo::cpp::file() const
{
	return out_file_;
}

