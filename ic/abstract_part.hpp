/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "core.hpp"


namespace modegen::ic::abstract {

class part : public generation_part {
	std::size_t id_;
	std::string name_;
	std::shared_ptr<input> data_;
public:
	part(std::size_t id, std::string name_in_config, std::shared_ptr<input> all_input);

	std::size_t id() const override;
	std::string name() const override;
	std::vector<std::shared_ptr<output>> outputs() const override;

	void rename(gen_utils::name_conversion to) override ;
	void split_versions() override ;
	void map_to(std::string_view tmpl) override;
};

} // namespace modegen::ic
