/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "provider.hpp"

namespace mg = modegen::generation;
using namespace std::literals;

mg::tmpl_gen_env::tmpl_gen_env(nlohmann::json data, const FS::path& tmpl)
	: gen_data_(std::move(data))
	, tmpl_path_(tmpl)
{
}

nlohmann::json& mg::tmpl_gen_env::data()
{
	return gen_data_;
}

const nlohmann::json& mg::tmpl_gen_env::data() const
{
	return gen_data_;
}


FS::path& mg::tmpl_gen_env::tmpl()
{
	return tmpl_path_;
}

const FS::path& mg::tmpl_gen_env::tmpl() const
{
	return tmpl_path_;
}


std::optional<FS::path> mg::tmpl_gen_env::out_file() const
{
	return out_file_;
}

mg::tmpl_gen_env& mg::tmpl_gen_env::out_file(const std::optional<FS::path>& p)
{
	out_file_ = p;
	return *this;
}

mg::tmpl_gen_env& mg::tmpl_gen_env::emb_fnc(const std::string& name, const script_descriptor& fnc)
{
	extra_scripts_[name] = fnc;
	return *this;
}

std::map<std::string, mg::tmpl_gen_env::script_descriptor> mg::tmpl_gen_env::emb_fnc_list() const
{
	std::map<std::string, script_descriptor> ret;
	for(auto s:extra_scripts_) if(s.first!="-"s && s.first!="+"s) ret[s.first] = s.second;
	return ret;
}

mg::tmpl_gen_env& mg::tmpl_gen_env::exec_after(const script_descriptor& fnc)
{
	extra_scripts_["+"s]=fnc;
	return *this;
}

mg::tmpl_gen_env& mg::tmpl_gen_env::exec_before(const script_descriptor& fnc)
{
	extra_scripts_["-"s]=fnc;
	return *this;
}

mg::tmpl_gen_env::script_descriptor mg::tmpl_gen_env::exec_after() const
{
	auto pos = extra_scripts_.find("+"s);
	if(pos==extra_scripts_.end()) return ""s;
	return pos->second;
}

mg::tmpl_gen_env::script_descriptor mg::tmpl_gen_env::exec_before() const
{
	auto pos = extra_scripts_.find("-"s);
	if(pos==extra_scripts_.end()) return ""s;
	return pos->second;
}
