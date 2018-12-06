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
}:

stdenv.mkDerivation rec {
	name = "modegen";
	version = "0.0.0";
	src = ./..;

	nativeBuildInputs = [
		# for generation
		python3 py_jinja bison2 flex
		# for build excutable file
		cmake ninja
		];
	buildInputs = [
		boost
		cppjson
	];
}
	
