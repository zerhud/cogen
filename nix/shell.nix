{
	  system ? builtins.currentSystem
	, pkgs ? import<nixos-unstable> { inherit system; }
	, compiler_name ? "gcc8"
	, enable_clcov ? false
}:

let
	turtle = import ./turtle.nix { inherit pkgs stdenv; } ;
	stdenv = pkgs.overrideCC pkgs.stdenv pkgs.${compiler_name};

in pkgs.callPackage ./default.nix {
	inherit stdenv turtle enable_clcov;
	py_jinja = pkgs.python3Packages.jinja2;
        clang = pkgs.clang_7;
}

