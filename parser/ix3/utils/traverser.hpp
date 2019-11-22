/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/file.hpp"

namespace ix3::utils {

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
	
class traverser {
public:
	traverser(const traverser&) =delete ;
	traverser& operator = (const traverser&) =delete ;

	traverser(traverser&&) =default ;
	traverser& operator = (traverser&&) =default ;

	traverser() noexcept =default ;
	virtual ~traverser() noexcept =default ;
protected:
	ast::module& module() ;
	void trav_module(const ast::module& mod) ;

	std::string path() const ;
	std::vector<ast::meta::set> meta_stack() const ;
private:
	virtual void on_obj(ast::module& obj) {}
	virtual void on_obj(ast::record& obj) {}
	virtual void on_obj(ast::function& obj) {}
	virtual void on_obj(ast::interface& obj) {}
	virtual void on_obj(ast::enumeration& obj) {}

	template<typename T, typename... Args>
	static std::string make_path(T&& v1, Args... v)
	{
		if constexpr(sizeof...(Args)==0) return v1;
		else return v1 + pdel + make_path(std::forward<Args>(v)...);
	}

	const static std::string pdel;

	std::string path_;
	ast::module cur_mod_;
	std::vector<ast::meta::set> mstack_;
};

} // namespace ix3::utils
