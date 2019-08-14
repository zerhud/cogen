{
	  system ? builtins.currentSystem
	, pkgs ? import<nixos-unstable> { inherit system; }
	, compiler_name ? "clang_7"
	, enable_clcov ? true
}:

let
	turtle = import ./turtle.nix { inherit pkgs stdenv; } ;
	stdenv = pkgs.overrideCC pkgs.stdenv pkgs.${compiler_name};

in pkgs.callPackage ./default.nix {
	inherit stdenv turtle enable_clcov;
	cquery = pkgs.cquery;
	py_jinja = pkgs.python3Packages.jinja2;
	clang = pkgs.clang_7;
}

