{  pkgs ? import <nixpkgs> {}
 , stdenv ? pkgs.stdenv
}:
stdenv.mkDerivation rec {
	name = "cppjson";
	meta = { description = "c++ utils for work with json."; };
	nativeBuildInputs = [ pkgs.cmake ];
	src = import ./tools/fossil_tarball.nix {
		name = "cppjson";
		fossil_hash = "76744fcb97dc4b023d8207402cb6e0d1321ddccf";
		sha256 = "198yzdm4kgxzaml57qpfdrjn7ppfsyxdg7a51qznn20f56m7qkxi";
	};
}

