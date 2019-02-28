/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>
#include <boost/property_tree/ptree.hpp>

namespace modegen::generation::options {

class container;
typedef std::shared_ptr<container> container_ptr;

enum class part_option {input, output, file_generator, naming};
enum class part_idl_filter {part_selection, mod_name, content_name, modificator};
enum class part_forwards {before, after, extends};
enum class template_option {versioning};

typedef std::variant<part_option, part_idl_filter, part_forwards, template_option> any_option;

class container {
	boost::property_tree::ptree opts;
public:
	/// returns option's help message
	static std::string description_message(any_option opt) ;

	boost::property_tree::ptree& raw() ;
	const boost::property_tree::ptree& raw() const ;

	template<typename T>
	T get(any_option key, const std::string& part="", const std::string& fgen="")
	{
	}

	template<typename T>
	std::optional<T> get_opt(any_option key, const std::string& part="", const std::string& fgen="")
	{
	}

	template<typename T>
	std::vector<T> get_vec(any_option key, const std::string& part="", const std::string& fgen="")
	{
	}
private:
	static std::string descr_message(part_forwards opt) ;
	static std::string descr_message(part_idl_filter opt) ;
	static std::string descr_message(part_option opt) ;
	static std::string descr_message(template_option opt) ;

	static std::string solve_key(any_option key) ;
	static std::string solve_key(part_forwards opt) ;
	static std::string solve_key(part_idl_filter opt) ;
	static std::string solve_key(part_option opt) ;
	static std::string solve_key(template_option opt) ;

	static std::string make_part_key(any_option key, const std::string& p) ;
	static std::string make_filegen_key(any_option key, const std::string& p) ;
	static std::string make_part_default_key(any_option key, const std::string& p) ;

	void throw_no_option(any_option key);
};

class view {
	container_ptr opts;
	std::string_view def_part;
	std::string_view def_fgen;
public:
	view(std::string_view p);

	void part(std::string_view p);
	void file_generator(std::string_view fg);
	std::string_view part() const ;
	std::string_view file_generator() const ;


	template<typename T>
	T get(any_option key, const std::string& part="", const std::string& fgen="")
	{
		return opts->get<T>(
				key,
				(part.empty() ? std::string(def_part) : part),
				(fgen.empty() ? std::string(def_fgen) : fgen)
				);
	}

	template<typename T>
	std::optional<T> get_opt(any_option key, const std::string& part="", const std::string& fgen="")
	{
		return opts->get_opt<T>(
				key,
				(part.empty() ? std::string(def_part) : part),
				(fgen.empty() ? std::string(def_fgen) : fgen)
				);
	}

	template<typename T>
	std::vector<T> get_vec(any_option key, const std::string& part="", const std::string& fgen="")
	{
		return opts->get_vec<T>(
				key,
				(part.empty() ? std::string(def_part) : part),
				(fgen.empty() ? std::string(def_fgen) : fgen)
				);
	}
};

} // namespace modegen::generation

