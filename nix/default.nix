{
	  stdenv
	, python3
	, py_jinja
	, bison2
	, flex
	, boost
	, cmake
}:

stdenv.mkDerivation rec {
	name = "modegen";
	version = "0.0.0";
	src = ./..;

	nativeBuildInputs = [
		# for generation
		python3 py_jinja bison2 flex
		# for build excutable file
		cmake 
		];
	buildInputs = [
		boost
	];
}
	
