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

boost::property_tree::ptree& mo::container::raw()
{
	return opts;
}

const boost::property_tree::ptree& mo::container::raw() const
{
	return opts;
}

std::vector<std::string> mo::container::part_list() const
{
	std::vector<std::string> ret;
	return ret;
}

boost::property_tree::ptree mo::container::get_subset(subsetts s, const std::string& part, const std::string& param) const
{
	boost::property_tree::ptree ret;
	auto t = opts.get_child_optional(make_subset_key(s, part, param));
	if(!t) t = opts.get_child_optional(make_subset_default_key(s, param));
	if(t) ret = *t;
	return ret;
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

std::string mo::container::solve_key(subsetts opt)
{
	if(opt==subsetts::part_data) return "";
	if(opt==subsetts::file_generator) return "file_gen"s;
	assert(false);
	return ""s;
}

mo::container::path_t mo::container::make_part_key(mo::any_option key, const std::string& p)
{
	return path_t("gen"s) / path_t(p) / path_t(solve_key(key));
}

mo::container::path_t mo::container::make_part_default_key(any_option key, const std::string& p)
{
	return path_t("defaults"s) / path_t(solve_key(key));
}

mo::container::path_t mo::container::make_subset_key(mo::subsetts key, const std::string& part, const std::string& param)
{
	path_t rpath = path_t("gen"s) / path_t(part) / path_t(solve_key(key), '.') ;
	if(!param.empty()) rpath /= path_t(param);
	return make_subset_default_key(key, param);
}

mo::container::path_t mo::container::make_subset_default_key(mo::subsetts key, const std::string& param)
{
	return path_t(solve_key(key)) / path_t(param);
}

std::tuple<mo::container::path_t,mo::container::path_t> mo::container::up_path(path_t p)
{
	path_t ret;
	while(!p.single()) ret /= p.reduce();
	return std::make_tuple(ret, p.reduce());
}

mo::view::view(container_ptr c, std::string_view p)
    : opts(std::move(c))
    , def_part(p)
{
}

void mo::view::part(std::string_view p)
{
	def_part = p;
}

void mo::view::file_generator(std::string_view fg)
{
	def_fgen = fg;
}

std::string_view mo::view::part() const
{
	return def_part;
}

std::string_view mo::view::file_generator() const
{
	return def_fgen;
}

boost::property_tree::ptree mo::view::get_subset(subsetts s, const std::string& param, const std::string& part) const
{
	return opts->get_subset(
	            s,
	            std::string(part.empty() ? def_part : part),
	            std::string(param.empty() ? def_fgen : param)
	            );
}

mo::forwards_view::forwards_view(mo::container_ptr o, std::string_view part)
    : opts(std::move(o))
    , def_part(part)
{
}

std::optional<mo::descriptor_t> mo::forwards_view::before() const
{
	std::optional<descriptor_t> ret;
	return ret;
}

std::optional<mo::descriptor_t> mo::forwards_view::after() const
{
	std::optional<descriptor_t> ret;
	return ret;
}

std::vector<mo::forwards_view::ex_descriptor> mo::forwards_view::ex_list() const
{
	std::vector<ex_descriptor> ret;
	return ret;
}

mo::filter_view::filter_view(mo::container_ptr o, std::string_view p)
    : opts(std::move(o))
	, def_part(p)
{
}

std::vector<mo::filter_view::info> mo::filter_view::in_part() const
{
	std::vector<info> ret;
	return ret;
}

