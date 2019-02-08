#include "loader.hpp"
#include "interface/loader.hpp"

using namespace std::literals;

modegen::parser::loader_ptr modegen::parser::create_loader(std::string_view name, std::vector<std::filesystem::path> includes)
{
	if(name == "interface"sv) return std::make_shared<interface::loader_impl>(std::move(includes));
	return nullptr;
}
