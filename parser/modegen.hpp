#pragma once

#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <boost/fusion/include/adapt_struct.hpp>

namespace modegen {
} // namespace modegen

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
		bool mut;
		std::vector<func_param> params;
	};
} // namespace modegen
BOOST_FUSION_ADAPT_STRUCT( modegen::func_param, (modegen::type, param_type), (std::string, name) )
BOOST_FUSION_ADAPT_STRUCT( modegen::function, (modegen::type, return_type), (std::string, name), (std::vector<modegen::func_param>, params), (bool, mut) )

namespace modegen {
	struct enumeration {
		int v;
		std::vector<std::string> elements;
		std::vector<std::string> ios;
		bool gen_comparator;
		bool gen_iterator;
		bool use_bitmask;
		bool gen_io;
	};
} // namespace modegen
BOOST_FUSION_ADAPT_STRUCT( modegen::enumeration, v, elements, ios,
	gen_comparator, gen_iterator, use_bitmask, gen_io )

namespace modegen {
	struct interface {
		int v;
	};
} // namespace modegen

namespace modegen {
	struct module {
		std::string name;
		std::vector<std::variant<function>> content;
	}; 
} // namespace modegen

BOOST_FUSION_ADAPT_STRUCT( modegen::module, name, content )
