#include "case_converter.h"

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
