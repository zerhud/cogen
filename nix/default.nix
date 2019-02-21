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

in
stdenv.mkDerivation rec {
	name = "modegen";
	version = "0.0.0";
	src = ./..;

	nativeBuildInputs = [ cmake ninja ] ++ clcov_deps;
	buildInputs = [
		# for generation
		python3 py_jinja
		# for build excutable file
		boost turtle pybind11
		cppjson
	];
}
	
