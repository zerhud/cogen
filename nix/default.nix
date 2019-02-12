{
	  stdenv
	, enable_clcov ? false
	, python3
	, py_jinja
	, bison2
	, flex
	, boost
	, cppjson
	, cmake
	, ninja
        , turtle
        , clang
        , llvm_7
}:

let
  clcov_deps = if enable_clcov then [clang llvm_7] else [];

in
stdenv.mkDerivation rec {
	name = "modegen";
	version = "0.0.0";
	src = ./..;

	nativeBuildInputs = [
		# for generation
		python3 py_jinja bison2 flex
		# for build excutable file
		cmake ninja
		] ++ clcov_deps;
	buildInputs = [
		boost turtle
		cppjson
	];
}
	
