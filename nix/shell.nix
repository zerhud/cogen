{
	  system ? builtins.currentSystem
	, pkgs ? import<nixos-unstable> { inherit system; }
	, compiler_name ? "clang_8"
	, enable_clcov ? true
}:

let
	turtle = import ./turtle.nix { inherit pkgs stdenv; } ;
        #stdenv = pkgs.overrideCC pkgs.stdenv pkgs.${compiler_name};
        stdenv = pkgs.llvmPackages_latest.stdenv;

in pkgs.callPackage ./default.nix {
	inherit stdenv turtle enable_clcov;
	cquery = pkgs.cquery;
	py_jinja = pkgs.python3Packages.jinja2;
	clang = pkgs.clang_8;
        llvm = pkgs.llvm_8;
}

