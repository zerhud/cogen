/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <cassert>
#include <algorithm>
#include "naming.hpp"

std::vector<std::string> gen_utils::split_name(const std::string &name)
{
	std::vector<std::string> ret;

	std::size_t prev_pos=0;
	std::size_t pos = name.find('_');
	while(pos!=name.npos) {
		ret.emplace_back(name.substr(prev_pos, pos-prev_pos));
		prev_pos = pos+1;
		pos = name.find('_',prev_pos);
		if(pos==name.npos) ret.emplace_back(name.substr(prev_pos));
	}

	if(ret.empty()) {
		std::string cur;
		for(std::size_t i=0;i<name.size();++i) {
			if(std::islower(name[i]) || std::isdigit(name[i])) cur += name[i];
			else {
				ret.emplace_back(cur);
				cur = name[i];
			}
		}
		ret.emplace_back(cur);
	}

	ret.erase(std::remove_if(ret.begin(),ret.end(),[](const std::string& a){return a.empty();}), ret.end());
	for(auto& r:ret) for(auto& s:r) s=std::tolower(s);

	return ret;
}

std::string gen_utils::convert(const std::string& name, name_conversion c)
{
	std::string ret = name;

	if(c==name_conversion::as_is) return ret;

	auto split = split_name(name);
	if(split.empty()) return ret;

	ret.clear();
	if(c == name_conversion::underscore) {
		for(auto& s:split) ret += s + "_";
		ret.erase(ret.size()-1,1);
	} else if(c==name_conversion::camel_case) {
		for(auto& s:split) {
			assert(!s.empty());
			s[0] = std::toupper(s[0]);
			ret += s;
		}
		ret[0] = std::tolower(ret[0]);
	} else if(c==name_conversion::title_case) {
		for(auto& s:split) {
			assert(!s.empty());
			s[0] = std::toupper(s[0]);
			ret += s;
		}
	} else {
		assert("not call name_conversion was used"==nullptr);
		ret = name;
	}

	return ret;
}

