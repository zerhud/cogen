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

std::string mo::container::description_message(mo::container::any_option opt)
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

std::string mo::container::descr_message(mo::container::part_forwards opt)
{
	if(opt==part_forwards::before) return "run script before the generation has began"s;
	if(opt==part_forwards::after) return "run script after the generation"s;
	if(opt==part_forwards::extends) return "extend generator by functions..."s;
	assert(false);
	return "unknown part forwards option"s;
}

std::string mo::container::descr_message(mo::container::part_idl_filter opt)
{
	if(opt==part_idl_filter::part_selection) return "select only it part: interface, record, function or enum"s;
	if(opt==part_idl_filter::mod_name) return "select only modules with name matched this tempalte"s;
	if(opt==part_idl_filter::content_name) return "select only contenet with name matched this tempalte"s;
	if(opt==part_idl_filter::modificator) return "script for modify data in each selected item"s;
	assert(false);
	return "unknown part idl filter option"s;
}

std::string mo::container::descr_message(mo::container::part_option opt)
{
	if(opt==part_option::input) return "tempalte file"s;
	if(opt==part_option::output) return "the generated file in result dir"s;
	if(opt==part_option::file_generator) return "which generator use to provide data tree"s;
	if(opt==part_option::naming) return "the name convension to use"s;
	assert(false);
	return "unknown part option"s;
}

std::string mo::container::descr_message(mo::container::template_option opt)
{
	return "split by versions: append version to module name; no: ignore version; enitity: append version to entity name"s;
}

