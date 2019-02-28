/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "options.hpp"

namespace mo = modegen::generation::options;

using namespace std::literals;

std::string mo::container::description_message(mo::any_option opt)
{
	if(std::holds_alternative<template_option>(opt)) {
		return descr_message(std::get<template_option>(opt));
	}

	if(std::holds_alternative<part_forwards>(opt)) {
		return descr_message(std::get<part_forwards>(opt));
	}

	if(std::holds_alternative<part_idl_filter>(opt)) {
		return descr_message(std::get<part_idl_filter>(opt));
	}

	if(std::holds_alternative<part_option>(opt)) {
		return descr_message(std::get<part_option>(opt));
	}

	assert(false);
	return "unknown option chapter"s;
}

std::string mo::container::descr_message(mo::part_forwards opt)
{
	if(opt==part_forwards::before) return "run script before the generation has began"s;
	if(opt==part_forwards::after) return "run script after the generation"s;
	if(opt==part_forwards::extends) return "extend generator by functions..."s;
	assert(false);
	return "unknown part forwards option"s;
}

std::string mo::container::descr_message(mo::part_idl_filter opt)
{
	if(opt==part_idl_filter::part_selection) return "select only it part: interface, record, function or enum"s;
	if(opt==part_idl_filter::mod_name) return "select only modules with name matched this tempalte"s;
	if(opt==part_idl_filter::content_name) return "select only contenet with name matched this tempalte"s;
	if(opt==part_idl_filter::modificator) return "script for modify data in each selected item"s;
	assert(false);
	return "unknown part idl filter option"s;
}

std::string mo::container::descr_message(mo::part_option opt)
{
	if(opt==part_option::input) return "tempalte file"s;
	if(opt==part_option::output) return "the generated file in result dir"s;
	if(opt==part_option::file_generator) return "which generator use to provide data tree"s;
	if(opt==part_option::naming) return "the name convension to use"s;
	assert(false);
	return "unknown part option"s;
}

std::string mo::container::descr_message(mo::template_option opt)
{
	return "split by versions: append version to module name; no: ignore version; enitity: append version to entity name"s;
}

std::string mo::container::solve_key(mo::any_option key)
{
	auto solver = [](const auto& k){ return solve_key(k); };
	return std::visit(solver, key);
}

std::string mo::container::solve_key(part_forwards opt)
{
	if(opt==part_forwards::before) return "forwards.before"s;
	if(opt==part_forwards::after) return "forwards.after"s;
	if(opt==part_forwards::extends) return "forwards.ex"s;
	assert(false);
	return ""s;
}

std::string mo::container::solve_key(part_idl_filter opt)
{
	if(opt==part_idl_filter::part_selection) return "filter.sel"s;
	if(opt==part_idl_filter::mod_name) return "filter.mod"s;
	if(opt==part_idl_filter::content_name) return "filter.cnt"s;
	if(opt==part_idl_filter::modificator) return "filter.applay"s;
	assert(false);
	return ""s;
}

std::string mo::container::solve_key(part_option opt)
{
	if(opt==part_option::input) return "input"s;
	if(opt==part_option::output) return "output"s;
	if(opt==part_option::file_generator) return "target"s;
	if(opt==part_option::naming) return "naming"s;
	assert(false);
	return ""s;
}

std::string mo::container::solve_key(template_option opt)
{
	if(opt==template_option::versioning) return "versioning"s;
	assert(false);
	return ""s;
}


std::string mo::container::make_part_key(mo::any_option key, const std::string& p)
{
	return "gen."s + p + solve_key(key);
}

std::string mo::container::make_filegen_key(any_option key, const std::string& p)
{
	return "filegen."s + p + solve_key(key);
}

std::string mo::container::make_part_default_key(any_option key, const std::string& p)
{
	return "defults."s + p + solve_key(key);
}

