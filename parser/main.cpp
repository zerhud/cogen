#include <fstream>
#include <iostream>
#include <functional>

#include "modegen.hpp"
#include "grammar.hpp"

void print_meta(const modegen::meta_parameters::parameter_set& params)
{
	std::cout << "\tmeta params:";
	if(!params.set.empty())  std::cout << std::endl;
	else {
		std::cout << " no" << std::endl;
		return;
	}

	auto  printer = [](const auto& p){ std::cout << "\t\t" << p.name() << ": " << p.value() << std::endl; };
	for(auto& param:params) std::visit(printer, param);
	std::cout << "\t--" << std::endl;
}

void print_mod(const modegen::module& mod)
{
	std::cout << mod.name << ":" << std::endl << "\t" << mod.content.size() << std::endl;
	print_meta(mod.meta_params);


	for(auto& cnt:mod.content) {
		auto* fnc = std::get_if<modegen::function>(&cnt);
		if(fnc) {
			std::cout << "\tfunc: " << fnc->return_type.name << " " << fnc->name << " ";
			if(fnc->is_mutable) std::cout << *fnc->is_mutable;
			else std::cout << "no_const" ;
			std::cout << " => ";
			for(auto& p:fnc->func_params) std::cout << p.param_type.name << " " << p.name << ", ";
			std::cout << std::endl;
			print_meta(fnc->meta_params);
		}
	}

	std::cout << std::endl;
}

int main(int,char**)
{
	std::fstream file("pdata");
	std::string pdata{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

	try{
		auto result = modegen::parse(pdata);
		for(auto& mod:result) print_mod(mod);
	}
	catch(const std::runtime_error& ex)
	{
		std::cerr << ex.what() << std::endl;
		return -1;
	}

	return 0;
}
