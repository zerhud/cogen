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

enum class output_lang { json, cpp, cmake, python, javascript };

class provider;
class generator;
class part_descriptor;
class output_descriptor;
class part_manager;
class final_generator;

typedef std::shared_ptr<provider> provider_ptr;
typedef std::shared_ptr<part_descriptor> part_descriptor_ptr;
typedef std::shared_ptr<output_descriptor> output_descriptor_ptr;

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
class filters_view;

} // namespace options

} // namespace modegen::pg

