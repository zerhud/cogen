/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <memory>
#include <vector>
#include <filesystem>
#include <exception>
#include "sddl_export.h"
#include "utils/generic_dsl.hpp"

namespace sddl {

class sddl_loader : public gen_utils::generic_sdl {
public:
	sddl_loader() = default ;
	bool empty() const override { return true; }
	std::string_view name() const override { return "sddl"; }
	void add(std::filesystem::path file) override {}
	void finish_loads() override {}
	gen_utils::tree data() const override {
		throw std::logic_error("not ready yet");
	}
};

} // namespace sddl

extern "C"  SDDL_EXPORT
gen_utils::generic_sdl_factory* create_dsl()
{
	struct inner_factory : gen_utils::generic_sdl_factory {
		generic_sdl_container languages(path_solver slv) const override
		{
			auto cnt = std::make_unique<sddl::sddl_loader>();
			generic_sdl_container ret;
			ret.emplace_back(std::move(cnt));
			return ret;
		}
	};

	static inner_factory ret;
	return &ret;
}
