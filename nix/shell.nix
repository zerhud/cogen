{
	  system ? builtins.currentSystem
	, pkgs ? import<nixpkgs> { inherit system; }
	, compiler_name ? "gcc8"
}:

pkgs.callPackage ./default.nix {
	py_jinja = pkgs.python3Packages.jinja2;
}

