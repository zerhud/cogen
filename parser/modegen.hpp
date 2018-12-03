#pragma once

#include <memory>
#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <boost/fusion/include/adapt_struct.hpp>

namespace modegen {
// templates for work with enums as flags
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T> operator~ (T a)      { return (T)(~(int)a); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T> operator| (T a, T b) { return (T)((int)a | (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T> operator& (T a, T b) { return (T)((int)a & (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T> operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T&> operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T&> operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T&> operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }
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
	version(std::uint64_t m, std::uint64_t i) noexcept ;

	version(version&&) noexcept =default ;
	version(const version&) noexcept =default ;
	version& operator = (version&& other) noexcept =default ;
	version& operator = (const version& other) noexcept =default ;

	std::uint64_t major_v;
	std::uint64_t minor_v;

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
	version since;

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

BOOST_FUSION_ADAPT_STRUCT( modegen::meta_parameters::version, (std::uint64_t, major_v), (std::uint64_t, minor_v) )
BOOST_FUSION_ADAPT_STRUCT( modegen::meta_parameters::documentation, (std::string, body) )
BOOST_FUSION_ADAPT_STRUCT( modegen::meta_parameters::deprication, (modegen::meta_parameters::version, since), (std::string, message) )

namespace modegen {
	struct type {
		type() =default ;
		type(const type& o) : name(o.name) {if(o.sub_type) sub_type = o.sub_type->clone();}
		type& operator = (const type& o) {name = o.name; sub_type = o.sub_type ? o.sub_type->clone() : nullptr; return *this;}

		std::unique_ptr<type> clone() const {
			auto ret = std::make_unique<type>();
			ret->name = name;
			if(sub_type) ret->sub_type = sub_type->clone();
			return ret;
		}

		std::string name;
		std::unique_ptr<type> sub_type;
	};
} // namespace modegen
BOOST_FUSION_ADAPT_STRUCT( modegen::type, name, sub_type )

namespace modegen {
	struct func_param {
		std::string name;
		type param_type;
	};

	struct function {
		std::string name;
		type return_type;
		std::optional<bool> is_mutable;
		std::optional<bool> is_static;
		std::vector<func_param> func_params;
		meta_parameters::parameter_set meta_params;
	};

	struct constructor_fnc {
		std::vector<func_param> func_params;
		meta_parameters::parameter_set meta_params;
	};
} // namespace modegen
BOOST_FUSION_ADAPT_STRUCT( modegen::func_param, (modegen::type, param_type), (std::string, name) )
BOOST_FUSION_ADAPT_STRUCT( modegen::constructor_fnc, func_params, meta_params )
BOOST_FUSION_ADAPT_STRUCT( modegen::function
                           , (modegen::type, return_type)
                           , (std::string, name)
                           , (std::vector<modegen::func_param>, func_params)
                           , (modegen::meta_parameters::parameter_set, meta_params)
                           , (std::optional<bool>, is_mutable)
                           , (std::optional<bool>, is_static)
                          )

namespace modegen {
	struct enum_element {
		std::string name;
		std::string io;
	};
	struct enumeration {
		std::string name;
		std::vector<enum_element> elements;
		meta_parameters::parameter_set meta_params;

		bool gen_io=false;
		bool use_bitmask=false;
	};
} // namespace modegen
BOOST_FUSION_ADAPT_STRUCT( modegen::enum_element, name, io )
BOOST_FUSION_ADAPT_STRUCT( modegen::enumeration, name, elements, meta_params, gen_io, use_bitmask )

namespace modegen {
	struct record_item {
		std::string name;
		type param_type;
		meta_parameters::parameter_set meta_params;
	};

	struct record {
		std::string name;
		std::vector<record_item> members;
		meta_parameters::parameter_set meta_params;
	};

	struct interface {
		std::string name;
		std::vector<function> mem_funcs;
		std::vector<constructor_fnc> constructors;
		meta_parameters::parameter_set meta_params;
		bool realization_in_client = false;
	};
} // namespace modegen
BOOST_FUSION_ADAPT_STRUCT( modegen::record_item, meta_params, param_type, name )
BOOST_FUSION_ADAPT_STRUCT( modegen::record, name, members, meta_params )
BOOST_FUSION_ADAPT_STRUCT( modegen::interface, name, mem_funcs, constructors, meta_params, realization_in_client )

namespace modegen {
typedef std::variant<function,enumeration,record,interface> module_content;
enum class module_content_selector{function = 1 << 0, enumeration = 1 << 1, record = 1 << 2, interface = 1 << 3, all = ~0};
struct using_directive {
	std::string mod_name;
};
struct module {
	std::string name;
	std::vector<module_content> content;
	meta_parameters::parameter_set meta_params;
	std::vector<using_directive> imports;
	std::string file_name;
};
} // namespace modegen

BOOST_FUSION_ADAPT_STRUCT(   modegen::using_directive, mod_name )
BOOST_FUSION_ADAPT_STRUCT(   modegen::module
                           , name
                           , content
                           , meta_params
                           , imports
                         )
