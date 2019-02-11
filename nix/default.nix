{
	  stdenv
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

stdenv.mkDerivation rec {
	name = "modegen";
	version = "0.0.0";
	src = ./..;

	nativeBuildInputs = [
		# for generation
		python3 py_jinja bison2 flex
		# for build excutable file
		cmake ninja clang llvm_7
		];
	buildInputs = [
		boost turtle
		cppjson
	];
}
	
