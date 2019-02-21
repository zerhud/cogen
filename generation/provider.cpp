/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "provider.hpp"

namespace mg = modegen::generation;

mg::tmpl_gen_data::tmpl_gen_data(cppjson::value data, const FS::path& tmpl)
	: gen_data_(std::move(data))
	, tmpl_path_(tmpl)
{
}

cppjson::value& mg::tmpl_gen_data::data()
{
	return gen_data_;
}

const cppjson::value& mg::tmpl_gen_data::data() const
{
	return gen_data_;
}


FS::path& mg::tmpl_gen_data::tmpl()
{
	return tmpl_path_;
}

const FS::path& mg::tmpl_gen_data::tmpl() const
{
	return tmpl_path_;
}


std::optional<FS::path> mg::tmpl_gen_data::out_dir() const
{
	return out_dir_;
}

mg::tmpl_gen_data& mg::tmpl_gen_data::out_dir(const std::optional<FS::path>& p)
{
	out_dir_ = p;
	return *this;
}

boost::property_tree::ptree  mg::tmpl_gen_data::generator_data() const
{
	return data_for_generator_;
}

mg::tmpl_gen_data& mg::tmpl_gen_data::generator_data(boost::property_tree::ptree d)
{
	data_for_generator_ = std::move(d);
	return *this;
}

