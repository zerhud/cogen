/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <filesystem>
#include "ast/file.hpp"
#include "utils/checker.hpp"

namespace ix3 {

class parser {
public:
	parser(const parser&) =delete ;
	parser& operator = (const parser&) =delete ;

	parser(parser&& other) noexcept =default ;
	parser& operator = (parser&& other) noexcept =default ;

	parser() noexcept ;
	parser(std::vector<std::filesystem::path> incs);
	virtual ~parser() noexcept =default ;

	void parse(std::istream& input, std::string fn) ;
	void parse(std::filesystem::path file) ;
	void finish_loads() ;

	void create_tree() ;

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
