/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string_view>
#include "core.hpp"

namespace modegen::ic::abstract {

class part : public generation_part {
	std::uint64_t id_{};
	std::string name_{};
	input data_{};
	std::pmr::vector<map_result> compiled_{};

	std::optional<std::pmr::string> replace(
			std::string_view tmpl, std::string_view name, std::string_view value) const;
	std::pmr::vector<map_result> create_input(
			std::pmr::string tmpl, const gen_utils::tree* data) const ;
public:
	part(std::uint64_t id,
		std::string name_in_config,
		input all_input);

	[[nodiscard]] std::uint64_t id() const override;
	[[nodiscard]] std::string_view name() const override;

	void rename(gen_utils::name_conversion to) override ;
	void split_versions() override ;
	void map_to(
			std::string_view tmpl) override;
	[[nodiscard]] std::pmr::vector<map_result> compiled_input() const override;
};

} // namespace modegen::ic
