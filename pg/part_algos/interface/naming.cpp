/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "naming.hpp"

#include <cassert>
#include <algorithm>

namespace mp = modegen::pg;
namespace mi = modegen::parser::interface;
namespace mg = modegen::pg::palgos::interface;

std::vector<std::string> mg::naming::split_name(const std::string &name)
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

std::string mg::naming::convert(const std::string& name, mp::name_conversion c)
{
	using pg::name_conversion;

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
		assert(("not call name_conversion was used",false));
		ret = name;
	}

	return ret;
}

mg::naming::naming(std::string_view sn) : naming(mp::from_string(std::string(sn)))
{
}

mg::naming::naming(mp::name_conversion c) : conver(c)
{
}

std::vector<mi::module>& mg::naming::operator() (std::vector<mi::module>& mods) const
{
	using namespace mi;
	using mp::name_conversion;

	struct {
		name_conversion naming = name_conversion::as_is;
		void convert(std::vector<module>& mods) { for(auto& m:mods) convert(m); }
		void convert(module& mod) {for(auto& c:mod.content) std::visit([this](auto& c){convert(c);}, c); }
		void convert(function& obj) { obj.name = naming::convert(obj.name, naming); }
		void convert(enumeration& obj) { obj.name = naming::convert(obj.name, naming); }
		void convert(record& obj) { obj.name = naming::convert(obj.name, naming); }
		void convert(mi::interface& obj) {
			if(naming!=name_conversion::camel_case) obj.name = naming::convert(obj.name, naming);
			else obj.name = naming::convert(obj.name, name_conversion::title_case);
			for(auto& m:obj.mem_funcs) convert(m);
		}
	} cvt;

	cvt.naming = conver;
	cvt.convert(mods);

	return mods;
}

