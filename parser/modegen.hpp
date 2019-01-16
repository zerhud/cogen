#pragma once

#define DO_PRAGMA(x) _Pragma (#x)
#ifdef NDEBUG
#define TODO(x)
#else
#define TODO(x) DO_PRAGMA(message ("TODO - " #x))
#endif

#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <boost/ptr_container/ptr_vector.hpp>
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
		type(const type& o) : name(o.name), sub_types(o.sub_types.begin(), o.sub_types.end()) {}
		type& operator = (const type& o) {name = o.name; sub_types.assign(o.sub_types.begin(),o.sub_types.end()); return *this;}

		std::unique_ptr<type> clone() const {
			auto ret = std::make_unique<type>();
			ret->name = name;
			ret->sub_types.assign(sub_types.begin(), sub_types.end());
			return ret;
		}

		std::string name;
		boost::ptr_vector<type> sub_types;
	};
} // namespace modegen
BOOST_FUSION_ADAPT_STRUCT( modegen::type, name, sub_types )

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

struct export_info {
	std::string name;
	module_content_selector type;
};
struct using_directive {
	std::string mod_name;
	bool is_system = false;
	std::vector<export_info> dest_items;

	bool is_from_system() const { return is_system; }
	bool is_from_other_module() const { return !is_from_system() && dest_items.empty(); }
	bool is_extra() const { return !is_from_system() && !dest_items.empty(); }
};
struct module {
	std::string name;
	std::vector<module_content> content;
	meta_parameters::parameter_set meta_params;
	std::vector<using_directive> imports;
	std::vector<export_info> exports;
	std::string file_name;
};

struct parsed_file {
	parsed_file() =default ;
	parsed_file(const std::vector<module>& m) : mods(m) {}

	std::vector<module> mods;
	std::vector<std::string> includes;
	std::string path;

	void push_back(module p);
	auto begin() {return mods.begin(); }
	auto end() {return mods.end(); }
	auto begin() const {return mods.begin(); }
	auto end() const {return mods.end(); }
};

} // namespace modegen

BOOST_FUSION_ADAPT_STRUCT(   modegen::using_directive, mod_name )
BOOST_FUSION_ADAPT_STRUCT(   modegen::parsed_file, mods, includes )
BOOST_FUSION_ADAPT_STRUCT(   modegen::module
                           , name
                           , content
                           , meta_params
                           , imports
                         )
