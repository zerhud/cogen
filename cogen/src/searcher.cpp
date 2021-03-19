/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "searcher.hpp"

using cogen::searcher;
namespace fs = std::filesystem;

std::ostream& cogen::operator << (std::ostream& out, const searcher& obj)
{
	out << "search in " << obj.pathes.size() << " directories" << std::endl;
	for(auto& p:obj.pathes) out << "\t" << p << std::endl;
	return out;
}

searcher::file_not_found::file_not_found(std::filesystem::path f, const searcher& solver)
{
	std::stringstream formatter;
	formatter
		<< "file " << f << " are not found"
		<< std::endl << solver << std::endl;
	text = formatter.str();
}

const char* searcher::file_not_found::what() const noexcept
{
	return text.c_str();
}

searcher& searcher::add(const std::filesystem::path& path)
{
	pathes.emplace_back(fs::canonical(path));
	return *this;
}

std::filesystem::path searcher::operator()(std::filesystem::path r) const
{
	if(r.is_absolute()) return r;
	for(auto& p:pathes) if(fs::exists(p/r)) return p/r;
	throw file_not_found(r, *this);
}

