{  pkgs ? import <nixpkgs> {}
 , stdenv ? pkgs.stdenv
}:
stdenv.mkDerivation rec {
  name = "turtle";
  meta = { description = "Boost like mock framework."; website = "https://github.com/mat007/turtle"; };
  buildCommand = "mkdir -p $out; cp -rt $out $src/include/*;";
  src = builtins.fetchGit {
    url = "https://github.com/mat007/turtle.git";
    name = "turtle.git";
    rev = "5c0f29012511339ba5cc2672f99a1356c5387b62";
  };
}
