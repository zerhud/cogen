{
	  stdenv
	, enable_clcov ? false
	, python3
	, py_jinja
	, boost
	, cmake
	, ninja
	, turtle
	, clang
	, llvm
	, pybind11
        , cquery
}:

let
  clcov_deps = if enable_clcov then [clang llvm cquery] else [];
  python3_diswar = python3.overrideAttrs( old : {
	  postPatch = old.postPatch + ''
	  sed -i '55d' Modules/_decimal/libmpdec/context.c
	  '';
  });
  pybind =
	  (pybind11.override { python = python3_diswar; })
	  .overrideAttrs( old : {
		  doCheck = true;
		  cmakeFlags = old.cmakeFlags ++ ["-DPYBIND11_TEST=OFF"];
	  })
  ;

in
stdenv.mkDerivation rec {
	name = "modegen";
	version = "0.0.0";
	src = ./..;

	nlohman_json_header = builtins.fetchurl {
		#url = "https://github.com/nlohmann/json/raw/develop/single_include/nlohmann/json.hpp";
		url = "https://github.com/nlohmann/json/raw/e326df211bd5b46c8ff9324cf093a932661da88a/single_include/nlohmann/json.hpp";
		sha256 = "0643h79nmrzrlky1am1x46ihdha7b11fxv44q4b86sm0m0z2y31v";
	};

	nativeBuildInputs = [ cmake ninja ] ++ clcov_deps;
	buildInputs = [
		# for generation
		python3_diswar py_jinja
		# for build excutable file
		boost turtle pybind
	];
}
	
