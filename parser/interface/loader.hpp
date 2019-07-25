/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "check.hpp"
#include "modegen.hpp"
#include "../loader.hpp"

namespace modegen::parser::interface {

class loader : public parser::loader {
public:
	virtual std::vector<module> result() const =0 ;
};

class loader_impl : public loader {
public:
	loader_impl();
	loader_impl(std::vector<FS::path> includes);

	void load(std::istream& input, std::string fn) override ;
	void load(FS::path file) override ;
	void finish_loads() override ;

	std::vector<module> result() const override ;
private:
	bool already_loaded(const FS::path f) const ;
	FS::path search_file(FS::path f) const ;

	std::vector<FS::path> incs;
	std::vector<FS::path> cur_dir;
	std::vector<FS::path> loaded_files;

	checker ch;
	std::vector<module> result_cache;
};

} // namespace modegen::parser::interface

