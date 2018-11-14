#pragma once

#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <boost/fusion/include/adapt_struct.hpp>

namespace modegen {
} // namespace modegen

namespace modegen {
class meta_parameter {
public:
	virtual ~meta_parameter() noexcept = default ;
	virtual std::string_view name() const =0;
	virtual std::string value() const =0;
	virtual bool add(const meta_parameter& other) =0 ;
};

namespace meta_parameters {
struct version : meta_parameter {
	version() noexcept ;
	explicit version(std::uint64_t v) noexcept ;
	version& operator = (std::uint64_t v) noexcept ;

	std::uint64_t val;
	std::string_view name() const override;
	std::string value() const override;
	bool add(const meta_parameter& other) override ;
};

struct documentation : meta_parameter {
	std::string body;

	std::string_view name() const override;
	std::string value() const override;
	bool add(const meta_parameter& other) override ;
};

struct deprication : meta_parameter {
	std::string message;

	std::string_view name() const override;
	std::string value() const override;
	bool add(const meta_parameter& other) override ;
};

struct parameter_set  {
	typedef std::variant<version,documentation,deprication> parameter_type;
	std::vector<parameter_type> set;

	void push_back(parameter_type p);
	auto begin() {return set.begin(); }
	auto end() {return set.end(); }
	auto begin() const {return set.begin(); }
	auto end() const {return set.end(); }
};

} // namespace meta_parameters
} // namespace modegen

BOOST_FUSION_ADAPT_STRUCT( modegen::meta_parameters::version, (std::uint64_t, val) )
BOOST_FUSION_ADAPT_STRUCT( modegen::meta_parameters::documentation, (std::string, body) )
BOOST_FUSION_ADAPT_STRUCT( modegen::meta_parameters::deprication, (std::string, message) )

namespace modegen {
	struct type {
		std::string name;
		std::string modificator;
	};
} // namespace modegen
BOOST_FUSION_ADAPT_STRUCT( modegen::type, (std::string, name), (std::string, modificator) )

namespace modegen {
	struct func_param {
		std::string name;
		type param_type;
	};

	struct function {
		std::string name;
		type return_type;
		std::optional<bool> mut;
		std::vector<func_param> func_params;
		meta_parameters::parameter_set meta_params;
	};
} // namespace modegen
BOOST_FUSION_ADAPT_STRUCT( modegen::func_param, (modegen::type, param_type), (std::string, name) )
BOOST_FUSION_ADAPT_STRUCT( modegen::function
                           , (modegen::type, return_type)
                           , (std::string, name)
                           , (std::vector<modegen::func_param>, func_params)
                           , (std::optional<bool>, mut)
                           , (modegen::meta_parameters::parameter_set, meta_params)
                          )

namespace modegen {
	struct enumeration {
		std::string name;
		std::vector<std::string> elements;
		std::vector<std::string> ios;
		meta_parameters::parameter_set meta_params;

		bool gen_io=false;
		bool use_bitmask=false;
	};
} // namespace modegen
BOOST_FUSION_ADAPT_STRUCT( modegen::enumeration, name, elements, ios, meta_params, gen_io, use_bitmask )

namespace modegen {
	struct interface {
		int v;
	};
} // namespace modegen

namespace modegen {
	struct module {
		std::string name;
		std::vector<std::variant<function,enumeration>> content;
		meta_parameters::parameter_set meta_params;
	}; 
} // namespace modegen

BOOST_FUSION_ADAPT_STRUCT(   modegen::module
                           , name
                           , content
                           , meta_params
                         )
