#include <regex>
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

	modegen::parser::loader_ptr create_loader(std::string_view target, FS::path input)
	{
		auto ldr = search_loader(target);
		if(!ldr) {
			ldr = create_loader(target);
			loaders[std::string(target)] = ldr;
		}

		assert(ldr);

		if(input=="-"sv) ldr->load(std::cin, "stdin");
		else ldr->load(input);

		return ldr;
	}

	modegen::parser::loader_ptr target_data(std::string_view name) const override
	{
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
	modegen::parser::loader_ptr search_loader(std::string_view name) const
	{
		auto pos = loaders.find(name);
		if(pos==loaders.end()) return nullptr;
		return pos->second;
	}

	modegen::parser::loader_ptr create_loader(std::string_view name) const
	{
		const bool is_interface = name == "cpp"sv || name == "interface"sv;
		if(is_interface) return std::make_shared<mi::loader_impl>();
		throw std::runtime_error("no such parser "s + std::string(name));
	}

	modegen::generation::jinja_python_generator pygen;
	std::map<std::string,modegen::parser::loader_ptr, std::less<>> loaders;
};

auto parse_command_line(int argc, char** argv, std::vector<std::string> glist)
{
	po::options_description desc("Allowed options");
	desc.add_options()
	        ("help,h", "produce this help message")
		("input,I", po::value<std::vector<std::string>>(), "input (foramt like -Iinterface=some_file). use - for read from std input")
		("generator,g", po::value<std::vector<std::string>>(), "info file for generation (file contains options)")
		("option,O", po::value<std::vector<std::string>>(), "override option from info file")
		("aoption", po::value<std::vector<std::string>>(), "add option to info file parser result")
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

	std::unique_ptr<mg::generator> gen;
	std::regex key_val_parser("([a-zA-Z_]+)(=(.+))?", std::regex::egrep);
	for(auto& opt:opts.options) {
		std::string& key = opt.string_key;
		std::string& val = opt.value[0];
		if(key=="input") {
			std::cmatch m;
			std::regex_match(val.data(), m, key_val_parser);
			FS::path input_path = m[3].str();
			if(input_path.is_relative()) input_path = FS::current_path() / input_path;
			prov->create_loader(m[1].str(), input_path);
		}
		else if(key=="generator") {
			gen = std::make_unique<mg::generator>(prov, val);
		}
		else if(key=="option") {
			if(!gen) throw std::runtime_error("cannot override option without generator");
			gen->options().put(key,val);
		}
		else if(key=="aoption") {
			if(!gen) throw std::runtime_error("cannot add option without generator");
			gen->options().add(key,val);
		}
		else if(key=="include") {
			TODO( add include directory );
		}
		else {
			std::cerr << u8"unknown option " << key << "=" << val << std::endl;
		}
	}

	return 0;
}

