#include <iostream>
#include <boost/program_options.hpp>
#include <boost/process.hpp>

#include "generation/common.hpp"
#include "generation/provider.hpp"
#include "generation/file_data.hpp"
#include "generation/pythongen.hpp"
#include "generation/cpp.hpp"

#include "parser/interface/loader.hpp"

namespace mg = modegen::generation;
namespace mi = modegen::parser::interface;
namespace po = boost::program_options;

using namespace std::literals;

class gen_prov : public mg::provider
{
public:
	gen_prov(FS::path self_path) : pygen(self_path.parent_path() / u8"pythongen")
	{
	}

	modegen::parser::loader_ptr target_data(std::string_view name) const override
	{
		const bool is_interface = name == "cpp"sv || name == "interface"sv;
		if(is_interface) return std::make_shared<mi::loader_impl>();
		return nullptr;
	}

	mg::file_data_ptr target_generator(std::string_view name) const override
	{
		if(name == "cpp"sv) return std::make_shared<mg::cpp_generator>();
		return nullptr;
	}

	void json_jinja(const cppjson::value& data, const FS::path& tmpl, const std::optional<FS::path>& out) const override
	{
		pygen(tmpl, out ? *out : u8"-", data);
	}

	std::vector<std::string> list_target() const
	{
		return { u8"interface" };
	}
private:
	modegen::generation::jinja_python_generator pygen;
};

auto parse_command_line(int argc, char** argv, std::vector<std::string> glist)
{
	po::options_description desc("Allowed options");
	desc.add_options()
	        ("help,h", po::value<std::vector<std::string>>(), "produce this help message")
		("intput,I", po::value<std::vector<std::string>>(), "input (foramt like -Iinterface=some_file). use - for read from std intput")
		("generator,g", po::value<std::vector<std::string>>(), "info file for generation (file contains options)")
		("option,O", po::value<std::vector<std::string>>(), "override option from info file")
		("include", po::value<std::vector<std::string>>(), "include directories for search files")
		;

	auto opts = po::command_line_parser(argc,argv).options(desc).run();
	po::variables_map vm;
	po::store(opts, vm);

	if(vm.count("help")) {
		std::cout << "use this program to prepare module data." << std::endl << desc << std::endl
			<< std::endl << "as target you may use:" << std::endl;
		for(auto& g:glist) std::cout << "\t" << g << std::endl;
		std::exit(1);
	}

	return std::make_tuple(opts, vm);
}

int main(int argc, char** argv)
{
	auto prov = std::make_shared<gen_prov>(argv[0]);
	auto [opts,vm] = parse_command_line(argc, argv, prov->list_target());

	for(auto& opt:opts.options) {
		std::string& key = opt.string_key;
		std::string& val = opt.value[0];
		if(key=="input") ;
		else if(key=="generator");
		else if(key=="option");
		else if(key=="include");
		else {
			std::cerr << u8"unknown option " << key << "=" << val << std::endl;
		}
	}

	return 0;
}

