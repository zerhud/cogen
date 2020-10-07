{ pkgs ? import<nixpkgs>{} }:
import ../hup/default.nix {
  inherit pkgs;
  stdenv = pkgs.gcc10Stdenv;
  #stdenv = pkgs.llvmPackages_10.stdenv;
  enable_clcov=true;
}
