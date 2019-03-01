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
enum class subsetts { file_generator };

typedef std::variant<part_option, part_idl_filter, part_forwards, template_option> any_option;

class container {
	boost::property_tree::ptree opts;
	typedef boost::property_tree::ptree::path_type path_t;
public:
	/// returns option's help message
	static std::string description_message(any_option opt) ;

	boost::property_tree::ptree& raw() ;
	const boost::property_tree::ptree& raw() const ;

	boost::property_tree::ptree get_subset(subsetts s, const std::string& part, const std::string& param) const ;

	template<typename T>
	T get(any_option key, const std::string& part, const std::string& fgen)
	{
		auto test_ret = opts.get_optional<T>(make_part_key(key, part));
		if(!test_ret) return opts.get<T>(make_part_default_key(key,part));
		return *test_ret;
	}

	template<typename T>
	std::optional<T> get_opt(any_option key, const std::string& part, const std::string& fgen)
	{
		auto test_ret = opts.get_optional<T>(make_part_key(key,part));
		if(!test_ret) test_ret = opts.get_optional<T>(make_part_default_key(key, part));
		return test_ret ? std::optional(*test_ret) : std::nullopt;
	}

	template<typename T>
	std::vector<T> get_vec(any_option key, const std::string& part, const std::string& fgen)
	{
		std::vector<T> ret;

		auto [part_f, part_s] = up_path(make_part_key(key,part));
		auto opt_child = opts.get_child_optional(part_f);
		if(!opt_child) {
			auto [def_f, def_s] = up_path(make_part_default_key(key,part));
			opt_child = opts.get_child_optional(def_f);
			part_s = def_s;
		}

		if(opt_child) {
			for(auto& opt:*opt_child) {
				if(opt.first==part_s.dump()) ret.emplace_back(opt.second.get_value<T>());
			}
		}

		return ret;
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
	static std::string solve_key(subsetts opt) ;

	static path_t make_part_key(any_option key, const std::string& p) ;
	static path_t make_part_default_key(any_option key, const std::string& p) ;
	static path_t make_subset_key(subsetts key, const std::string& part, const std::string& param) ;
	static path_t make_subset_default_key(subsetts key, const std::string& p) ;

	static std::tuple<path_t,path_t> up_path(path_t p);

	void throw_no_option(any_option key);
};

class view {
	container_ptr opts;
	std::string_view def_part;
	std::string_view def_fgen;
public:
	view(container_ptr c, std::string_view p);

	void part(std::string_view p);
	void file_generator(std::string_view fg);
	std::string_view part() const ;
	std::string_view file_generator() const ;

	boost::property_tree::ptree get_subset(subsetts s, const std::string& param="", const std::string& part="") const ;

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

