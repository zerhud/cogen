/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <ostream>
#include <filesystem>

namespace mdg2 {

class searcher;
std::ostream& operator << (std::ostream& out, const searcher& obj);

class searcher {
	friend std::ostream& operator << (std::ostream& out, const searcher& obj);

	std::vector<std::filesystem::path> pathes;
public:
	struct file_not_found : std::exception {
		std::string text;
		file_not_found(std::filesystem::path f, const searcher& solver);
		const char* what() const noexcept override ;
	};

	searcher& add(const std::filesystem::path& path);
	std::filesystem::path operator()(std::filesystem::path r) const ;
};


} // namespace mdg2

