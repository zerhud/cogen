/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <iostream>
#include <boost/property_tree/json_parser.hpp>
#include "options.hpp"
#include "exceptions.hpp"

namespace mo = modegen::pg::options;

using namespace std::literals;

std::string mo::container::description_message(mo::any_option opt)
{
	if(std::holds_alternative<template_option>(opt)) {
		return descr_message(std::get<template_option>(opt));
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

mo::container::container(FS::path of)
    : opts_file(std::move(of))
{
}

FS::path mo::container::opts_dir() const
{
	return opts_file.parent_path();
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
	auto gchild = opts.get_child_optional("part");
	if(gchild) for(auto& ps:*gchild) ret.emplace_back(ps.first);
	return ret;
}

boost::property_tree::ptree mo::container::get_subset(subsetts s, const std::string& part, const std::string& param) const
{
	boost::property_tree::ptree ret;
	auto t = opts.get_child_optional(make_subset_key(s, part, param));
	if(!t && !is_toplevel_subset(s)) t = opts.get_child_optional(make_subset_default_key(s, param));
	if(t) ret = *t;
	return ret;
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
	if(opt==part_option::output_name_gen) return "output file name generator"s;
	if(opt==part_option::file_generator) return "which generator use to provide data tree"s;
	if(opt==part_option::naming) return "the name convension to use"s;
	if(opt==part_option::output_lang) return "output language"s;
	assert(false);
	return "unknown part option"s;
}

std::string mo::container::descr_message(mo::template_option opt)
{
	if(opt==template_option::versioning)
		return "split by versions: append version to module name; no: ignore version; enitity: append version to entity name"s;
	if(opt==template_option::modularization)
		return "split: module per file; combine: all modules in one file"s;
	assert(false);
	return "unknown tempalte options"s;
}

std::string mo::container::solve_key(mo::any_option key)
{
	auto solver = [](const auto& k){ return solve_key(k); };
	return std::visit(solver, key);
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
	if(opt==part_option::output_name_gen) return "output_ng"s;
	if(opt==part_option::file_generator) return "filegen"s;
	if(opt==part_option::naming) return "naming"s;
	if(opt==part_option::output_lang) return "lang"s;
	assert(false);
	return ""s;
}

std::string mo::container::solve_key(template_option opt)
{
	if(opt==template_option::versioning) return "versioning"s;
	if(opt==template_option::modularization) return "modularization"s;
	assert(false);
	return ""s;
}

std::string mo::container::solve_key(subsetts opt)
{
	if(opt==subsetts::part_data) return ""s;
	if(opt==subsetts::file_generator) return "filegen"s;
	if(opt==subsetts::part_forwards) return "forwards"s;
	assert(false);
	return ""s;
}

mo::container::path_t mo::container::make_part_key(mo::any_option key, const std::string& p)
{
	return path_t("part"s) / path_t(p) / path_t(solve_key(key));
}

mo::container::path_t mo::container::make_part_default_key(any_option key, const std::string& p)
{
	(void)p;
	return path_t("defaults"s) / path_t(solve_key(key));
}

mo::container::path_t mo::container::make_subset_key(mo::subsetts key, const std::string& part, const std::string& param)
{
	if(is_toplevel_subset(key)) return path_t(solve_key(key)) / path_t(param);

	path_t rpath = path_t("part"s) / path_t(part) / path_t(solve_key(key), '.') ;
	if(!param.empty()) rpath /= path_t(param);
	return rpath;
}

mo::container::path_t mo::container::make_subset_default_key(mo::subsetts key, const std::string& param)
{
	assert( !is_toplevel_subset(key) );
	return path_t("defaults"s) / path_t(solve_key(key)) / path_t(param);
}

bool mo::container::is_toplevel_subset(mo::subsetts key)
{
	return key == subsetts::file_generator;
}

std::tuple<mo::container::path_t,mo::container::path_t> mo::container::up_path(path_t p)
{
	path_t ret;
	while(!p.single()) ret /= p.reduce();
	return std::make_tuple(ret, p.reduce());
}

mo::part_view::part_view(container_ptr c, std::string_view p)
    : opts(std::move(c))
    , def_part(p)
{
}

void mo::part_view::part(std::string_view p)
{
	def_part = p;
}

void mo::part_view::file_generator(std::string_view fg)
{
	def_fgen = fg;
}

std::string mo::part_view::output_mode() const
{
	auto sub = get_subset(subsetts::part_data);
	for(auto& s:sub) {
		if(s.first == "file_bymod"s) return s.first;
		if(s.first == "file_byent"s) return s.first;
		if(s.first == "file_single"s) return s.first;
		if(s.first == "file"s) return s.first;
	}

	throw errors::error("no output found for part "s + std::string(def_part));
}

std::string mo::part_view::output_tmpl() const
{
	std::string key = "part."s + std::string(part()) + '.' + output_mode();
	return opts->raw().get<std::string>(key);
}

std::string_view mo::part_view::part() const
{
	return def_part;
}

std::string_view mo::part_view::file_generator() const
{
	return def_fgen;
}

boost::property_tree::ptree mo::part_view::get_subset(subsetts s, const std::string& param, const std::string& part) const
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
	part_view v(opts, def_part);
	auto fwd_data = v.get_subset(subsetts::part_forwards);
	auto bdata = fwd_data.get_child_optional("before"s);
	if(bdata) ret = extract_desc(*bdata);
	//for(auto fwd:fwd_data) if(fwd.frist == "before"s) ret.emplace_back(extract_desc(fwd.second));
	return ret;
}

std::optional<mo::descriptor_t> mo::forwards_view::after() const
{
	std::optional<descriptor_t> ret;
	part_view v(opts, def_part);
	auto fwd_data = v.get_subset(subsetts::part_forwards);
	auto bdata = fwd_data.get_child_optional("after"s);
	if(bdata) ret = extract_desc(*bdata);
	//for(auto fwd:fwd_data) if(fwd.frist == "after"s) ret.emplace_back(extract_desc(fwd.second));
	return ret;
}

std::vector<mo::forwards_view::ex_descriptor> mo::forwards_view::ex_list() const
{
	std::vector<ex_descriptor> ret;
	part_view v(opts, def_part);
	auto fwd_data = v.get_subset(subsetts::part_forwards);
	auto ex_child = fwd_data.get_child_optional("ex"s);
	if(ex_child) for(auto ex:*ex_child) {
		ex_descriptor& item = ret.emplace_back();
		item.name = ex.second.get<std::string>("name"s);
		item.source = extract_desc(ex.second);
	}
	return ret;
}

mo::descriptor_t mo::forwards_view::extract_desc(const boost::property_tree::ptree& pt) const
{
	auto file = pt.get_optional<std::string>("file"s);
	auto script = pt.get_optional<std::string>("script"s);
	if(file) return FS::path(*file);
	return *script;
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

