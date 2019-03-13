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

namespace modegen {
namespace generation {

/// generation environment
class tmpl_gen_env ;

class provider; ///< provides data for generation
typedef std::shared_ptr<provider> provider_ptr;

class file_data ; ///< provides data for concreate file generation
typedef std::shared_ptr<file_data> file_data_ptr;

/// describes generation part, contains extra info, provied by file generator
/// or contains just part info from settings if no special part generation are used
class part_descrioptor;

/// common generator: reads input settings (from info file for example), and
/// create output with file_data generators, creates generation environment
class generator;

class tmpl_gen_env; ///< environment for generation
/// data provider. provides data for generation and provides access to system.
/// used by \reg generator.
class provider;

/// helpers and abstractions for read options.
/// for access to options should be used clases from this namespace.
namespace options {

/// common options container, main abstraction
/// for access to options.
class container;
typedef std::shared_ptr<container> container_ptr;
typedef std::variant<std::string, FS::path> descriptor_t;

class view; ///< view for part
class forwards_view;
class filters_view;

} // namespace options

} // namespace generation
} // namespace modegen
