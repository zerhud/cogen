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
	std::shared_ptr<configuration> data_{};
public:
	part(std::uint64_t id,
		std::string name_in_config,
		std::shared_ptr<configuration> all_input);

	[[nodiscard]] std::uint64_t id() const override;
	[[nodiscard]] std::string_view name() const override;

	void rename(gen_utils::name_conversion to) override ;
	void split_versions() override ;
	[[nodiscard]] std::pmr::vector<map_result> map_to(
			std::string_view tmpl) override;
};

} // namespace modegen::ic
