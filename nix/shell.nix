{
	  system ? builtins.currentSystem
	, pkgs ? import<nixos-unstable> { inherit system; }
	, compiler_name ? "gcc8"
}:

let
	cppjson = import ./cppjson.nix {};
        turtle = import ./turtle.nix { inherit pkgs stdenv; } ;
	stdenv = pkgs.overrideCC pkgs.stdenv pkgs.${compiler_name};

in pkgs.callPackage ./default.nix {
	inherit cppjson stdenv turtle;
	py_jinja = pkgs.python3Packages.jinja2;
}

