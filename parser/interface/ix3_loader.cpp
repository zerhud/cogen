/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ix3_loader.hpp"

#include "ix3/utils/traverser.hpp"
#include "ix3/utils/checker.hpp"
#include "ix3/utils/meta.hpp"

#include "ix3/parser.hpp"

namespace mpi = modegen::parser::interface;

namespace modegen::parser::interface::helpers {

struct ix3_to_modegen : ix3::utils::traverser {
	std::vector<mpi::module> result;
	mpi::interface* cur_iterface=nullptr;

	void convert(std::vector<ix3::ast::module> ix3_mods);
private:
	void on_obj(ix3::ast::module& obj) override ;
	void on_obj(ix3::ast::record& obj) override ;
	void on_obj(ix3::ast::function& obj) override ;
	void on_obj(ix3::ast::interface& obj) override ;
	void on_obj(ix3::ast::enumeration& obj) override ;
	void on_obj(ix3::ast::record_item& obj) override ;

	mpi::type cvt(const ix3::ast::type& obj) const ;
	mpi::meta_parameters::parameter_set cvt(const ix3::ast::meta::set& obj) const ;
};

void ix3_to_modegen::convert(std::vector<ix3::ast::module> ix3_mods)
{
	for(auto& m:ix3_mods) trav_module(m, trav_direction::paret_first);
}

void ix3_to_modegen::on_obj(ix3::ast::module& obj)
{
	mpi::module& cm = result.emplace_back(mpi::module{});
	cm.name = obj.name;

	cm.meta_params = cvt(obj.meta_params);
}

void ix3_to_modegen::on_obj(ix3::ast::record& obj)
{
	mpi::record cr;
	cr.name = obj.name;
	cr.use_as_exception = obj.use_as_exception;
	cr.meta_params = cvt(obj.meta_params);

	result.back().content.emplace_back(std::move(cr));
}

void ix3_to_modegen::on_obj(ix3::ast::function& obj)
{
	mpi::function cf;
	cf.name = obj.name;
	cf.is_static = obj.is_static;
	cf.is_mutable = obj.is_mutable;
	cf.return_type = cvt(obj.return_type);
	cf.meta_params = cvt(obj.meta_params);
	for(auto& p:obj.params)
		cf.func_params.emplace_back(mpi::func_param{p.name, cvt(p.param_type)});

	result.back().content.emplace_back(std::move(cf));
}

void ix3_to_modegen::on_obj(ix3::ast::interface& obj)
{
	mpi::interface ci;
	ci.name = obj.name;
	ci.use_as_exception = obj.use_as_exception;
	ci.realization_in_client = obj.realization_in_client;
	ci.meta_params = cvt(obj.meta_params);

	result.back().content.emplace_back(std::move(ci));
}

void ix3_to_modegen::on_obj(ix3::ast::enumeration& obj)
{
	mpi::enumeration ce;
	ce.name = obj.name;
	ce.gen_io = obj.gen_io;
	ce.use_bitmask = obj.use_bitmask;
	for(auto& e:obj.elements)
		ce.elements.emplace_back(mpi::enum_element{e.name, e.io});
	ce.meta_params = cvt(obj.meta_params);

	result.back().content.emplace_back(std::move(ce));
}

void ix3_to_modegen::on_obj(ix3::ast::record_item& obj)
{
	mpi::record_item cri;
	cri.name = obj.name;
	cri.param_type = cvt(obj.param_type);

	mpi::record& rec = std::get<mpi::record>(result.back().content.back());

	cri.meta_params = cvt(obj.meta_params);

	rec.members.emplace_back(std::move(cri));
}

mpi::type ix3_to_modegen::cvt(const ix3::ast::type &obj) const
{
	assert(!obj.name.empty());

	mpi::type ret;
	ret.name = obj.name[0];
	for(auto& st:obj.sub_types)
		ret.sub_types.push_back(new mpi::type(cvt(st.get())));

	return ret;
}

mpi::meta_parameters::parameter_set ix3_to_modegen::cvt(const ix3::ast::meta::set& obj) const
{
	using namespace ix3::ast::meta;
	mpi::meta_parameters::parameter_set ret;

	for(auto& mp:obj.cnt) {
		const oapi* moapi = boost::get<oapi>(&mp.var);
		const version* mver = boost::get<version>(&mp.var);
		const depricated* mdep = boost::get<depricated>(&mp.var);
		const documentation* mdoc = boost::get<documentation>(&mp.var);
		if(mver) {
			mpi::meta_parameters::version mm;
			mm.major_v = mver->major_v;
			mm.minor_v = mver->minor_v;
			ret.push_back(std::move(mm));
		} else if(mdep) {
			mpi::meta_parameters::deprication mm;
			mm.message = mdep->message;
			ret.push_back(std::move(mm));
		} else if(mdoc) {
			mpi::meta_parameters::documentation mm;
			mm.body = mdoc->body;
			ret.push_back(std::move(mm));
		} else if(moapi) {
			mpi::meta_parameters::oapi mm;
			mm.key = moapi->key;
			mm.val = moapi->val;
			ret.push_back(std::move(mm));
		}
	}

	return ret;
}

} // namespace modegen::parser::interface::helpers

class mpi::ix3_loader::details {
public:
	details() =default ;
	details(std::vector<std::filesystem::path> incs) : ix_parser(std::move(incs)) {}

	ix3::parser ix_parser;
};

modegen::parser::interface::ix3_loader::ix3_loader()
    : pimpl_(std::make_unique<details>())
{
}

modegen::parser::interface::ix3_loader::ix3_loader(std::vector<std::filesystem::path> includes)
    : pimpl_(std::make_unique<details>(std::move(includes)))
{
}

modegen::parser::interface::ix3_loader::~ix3_loader() noexcept
{
}

void modegen::parser::interface::ix3_loader::load(std::istream& input, std::string fn)
{
	pimpl().ix_parser.parse(input, std::move(fn));
}

void modegen::parser::interface::ix3_loader::load(std::filesystem::path file)
{
	pimpl().ix_parser.parse(std::move(file));
}

void modegen::parser::interface::ix3_loader::finish_loads()
{
	pimpl().ix_parser.finish_loads();
}

std::vector<modegen::parser::interface::module> modegen::parser::interface::ix3_loader::result() const
{
	std::vector<mpi::module> result;
	auto ix3_result = pimpl().ix_parser.result();

	helpers::ix3_to_modegen cvt;
	cvt.convert(ix3_result);
	cvt.result.swap(result);

	return result;
}

std::vector<ix3::ast::module> modegen::parser::interface::ix3_loader::ix3_result() const
{
	return pimpl().ix_parser.result();
}

