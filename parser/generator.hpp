#pragma once

#include <map>
#include <functional>
#include <filesystem>
#include <string_view>
#include <type_traits>

#include "modegen.hpp"

namespace modegen {

class generator;
typedef std::function<std::unique_ptr<generator>()> generator_creator;

enum class gen_options {
	  no_opts = 1 << 0
	, split_version = 1 << 1
};

template<class T> inline std::enable_if_t<std::is_enum_v<T>,int> operator~ (T a)      { return ~(int)a; }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,int> operator| (T a, T b) { return ((int)a | (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,int> operator& (T a, T b) { return ((int)a & (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,int> operator^ (T a, T b) { return ((int)a ^ (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T&> operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T&> operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T&> operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

//inline gen_options operator | (gen_options a, gen_options b)
//{
    //int ret = static_cast<int>(a) | static_cast<int>(b);
    //return static_cast<gen_options>(ret);
//}

void cast_options(gen_options opts, std::vector<modegen::module>& mods);

class generator {
public:
	virtual ~generator() noexcept =default ;

	virtual gen_options options() const =0 ;
	virtual void options(gen_options opts) =0 ;
	virtual void output(std::filesystem::path o) =0 ; ///< dir or file
	virtual void generate(std::vector<modegen::module> mods) const =0 ;
};

class generator_maker {
public:
	void register_gen(std::string_view part, std::string_view name, generator_creator crt);
	std::unique_ptr<generator> make_generator(std::string_view part, std::string_view name) const ;
private:
	static std::string make_path(std::string_view part, std::string_view name) ;
	std::map<std::string, generator_creator> gens;
};

} // namespace modegen
