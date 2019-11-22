/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <filesystem>
#include "ast/file.hpp"
#include "utils/checker.hpp"

namespace ix3 {

class loader {
public:
	loader(const loader&) =delete ;
	loader& operator = (const loader&) =delete ;

	loader(loader&& other) noexcept =default ;
	loader& operator = (loader&& other) noexcept =default ;

	loader() noexcept ;
	loader(std::vector<std::filesystem::path> incs);
	virtual ~loader() noexcept =default ;

	void load(std::istream& input, std::string fn) ;
	void load(std::filesystem::path file) ;
	void finish_loads() ;

	std::vector<ix3::ast::module> result() const ;
private:
	ast::file_content parse_stream(std::istream& input) const ;
	std::filesystem::path search_file(const std::filesystem::path& f) const ;
	bool already_loaded(const std::filesystem::path& p) const ;

	std::vector<std::filesystem::path> includes_dir;
	std::vector<std::filesystem::path> cur_dir;
	std::vector<std::filesystem::path> loaded_files;

	utils::checker checker;
	std::vector<ast::module> finished_ast;
};

} // namespace ix3
