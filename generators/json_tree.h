#pragma once

#include "generator.hpp"

namespace modegen {
namespace generators {

class json_tree : public modegen::generator
{
public:
	gen_options options() const override ;
	void options(gen_options opts) override ;
	void output(std::filesystem::path o) override ;
	void generate(std::vector<modegen::module> mods) const override ;
private:
	gen_options gopts;
	std::filesystem::path output_file;
};

} // namespace generators
} // namespace modegen
