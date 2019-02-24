{
	  stdenv
	, enable_clcov ? false
	, python3
	, py_jinja
	, boost
	, cppjson
	, cmake
	, ninja
	, turtle
	, clang
	, llvm_7
	, pybind11
}:

let
  clcov_deps = if enable_clcov then [clang llvm_7] else [];
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

	nativeBuildInputs = [ cmake ninja ] ++ clcov_deps;
	buildInputs = [
		# for generation
		python3_diswar py_jinja
		# for build excutable file
		boost turtle pybind
		cppjson
	];
}
	
