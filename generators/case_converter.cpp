#include "case_converter.h"

#include <cassert>
#include <algorithm>

std::vector<std::string> modegen::generators::split_name(const std::string &name)
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
			if(std::islower(name[i])) cur += name[i];
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

std::string modegen::generators::convert(const std::string &name, modegen::generators::name_conversion c)
{
	std::string ret;

	if(c==name_conversion::as_is) return ret;

	auto split = split_name(name);
	if(split.empty()) return ret;

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

std::string_view modegen::generators::to_string(modegen::generators::name_conversion c)
{
	using namespace std::literals;
	if(c==name_conversion::as_is) return "asis"sv;
	if(c==name_conversion::underscore) return "underscore"sv;
	if(c==name_conversion::camel_case) return "camel"sv;
	if(c==name_conversion::title_case) return "title"sv;
	assert(false);
	return "asis"sv;
}

modegen::generators::name_conversion modegen::generators::from_string(std::string_view n)
{
	using namespace std::literals;
	if(n=="underscore"sv) return name_conversion::underscore;
	if(n=="camel"sv) return name_conversion::camel_case;
	if(n=="title"sv) return name_conversion::title_case;
	return name_conversion::as_is;
}

void modegen::generators::convert(std::vector<modegen::module>& mods, modegen::generators::name_conversion c)
{
	struct {
		name_conversion naming = name_conversion::as_is;
		void convert(std::vector<module>& mods) { for(auto& m:mods) convert(m); }
		void convert(module& mod) {for(auto& c:mod.content) std::visit([this](auto& c){convert(c);}, c); }
		void convert(function& obj) {}
		void convert(enumeration& obj) {}
		void convert(record& obj) {}
		void convert(interface& obj) {}
	} cvt;

	cvt.naming = c;
	cvt.convert(mods);
}
