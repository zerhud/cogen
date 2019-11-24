/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
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
	enum class trav_direction { paret_first, child_first } ;
	typedef std::variant<ast::module*, ast::record*, ast::interface*> parent_t;

	ast::module& module() ;
	void trav_module(const ast::module& mod, trav_direction direction = trav_direction::child_first) ;

	std::string path() const ;
	std::vector<ast::meta::set> meta_stack() const ;

	template<typename P>
	P* parent(std::size_t ind)
	{
		if(parents_.size() <= ind) return nullptr;
		std::size_t pind = parents_.size() - ind - 1;
		if(std::holds_alternative<P*>(parents_[pind]))
			return std::get<P*>(parents_[pind]);
		return nullptr;
	}
private:
	virtual void on_obj(ast::module& obj)      { (void)obj; }
	virtual void on_obj(ast::record& obj)      { (void)obj; }
	virtual void on_obj(ast::function& obj)    { (void)obj; }
	virtual void on_obj(ast::interface& obj)   { (void)obj; }
	virtual void on_obj(ast::enumeration& obj) { (void)obj; }
	virtual void on_obj(ast::record_item& obj) { (void)obj; }
	virtual void on_obj(ast::constructor& obj) { (void)obj; }

	template<typename T, typename... Args>
	std::string make_path(T&& v1, Args... v)
	{
		if constexpr(sizeof...(Args)==0) path_ = v1;
		else path_ = v1 + pdel + make_path(std::forward<Args>(v)...);
		return path_;
	}

	const static std::string pdel;

	template<typename O, typename Fnc>
	void inner_trav(O& obj, const Fnc& fnc)
	{
		make_path(cur_mod_.name, obj.name);
		if(cur_direction==trav_direction::paret_first)
			on_obj(obj);

		parents_.emplace_back(&obj);
		fnc();
		parents_.pop_back();

		if(cur_direction==trav_direction::child_first)
			on_obj(obj);
	}

	trav_direction cur_direction = trav_direction::child_first;

	std::string path_;
	ast::module cur_mod_;
	std::vector<ast::meta::set> mstack_;
	std::vector<parent_t> parents_;
};

} // namespace ix3::utils
