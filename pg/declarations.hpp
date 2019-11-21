/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

/// this file contains forwards declarations of enitities of generation
/// part of modegen project

#pragma once

#include "config.hpp"

/// filesystem are not supported by clang
/// so may be used different filesystem's implementations
/// the \ref FILESYSTEM macro defined in config.hpp
#include FILESYSTEM

#include <memory>
#include <variant>
#include <string_view>

namespace modegen::pg {

enum class  input_lang { mdl, data };
std::string to_string(input_lang l);

enum class output_lang { json, cpp, cmake, python, javascript };
std::string to_string(output_lang l);
inline std::ostream& operator << (std::ostream& out, output_lang o) { return out << to_string(o); }

enum class name_conversion{underscore, camel_case, title_case, as_is};
std::string_view to_string(name_conversion c);
inline std::ostream& operator << (std::ostream& out, name_conversion o) { return out << to_string(o); }

class provider;
class generator;
class part_descriptor;
class output_descriptor;
class part_manager;
class final_generator;
class part_algos;

struct jinja_env;

typedef std::shared_ptr<provider> provider_ptr;
typedef std::shared_ptr<const provider> provider_const_ptr;
typedef std::shared_ptr<part_descriptor> part_descriptor_ptr;
typedef std::shared_ptr<output_descriptor> output_descriptor_ptr;
typedef std::shared_ptr<part_algos> part_algos_ptr;

/// helpers and abstractions for read options.
/// for access to options should be used clases from this namespace.
namespace options {

/// common options container, main abstraction
/// for access to options.
class container;
typedef std::shared_ptr<container> container_ptr;
typedef std::variant<std::string, FS::path> descriptor_t;

class part_view; ///< view for part
class forwards_view;
class filter_view;

} // namespace options

/// conversion from string by return type.
/// this class must be used as a function.
class from_string {
	// use std::string insteed of string_view cause
	// the temporaty variable can to be destroyed before operator call
	// (also it can be destroyed after ; operator, but we cannot garantee it)
	// it is normal for small string
	std::string val;
public:
	from_string(std::string s);
	operator output_lang() const ;
	operator name_conversion() const ;
};

} // namespace modegen::pg
