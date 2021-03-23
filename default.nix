{ nixpkgs ? import<nixpkgs>{},
  stdenv ? nixpkgs.gcc10Stdenv,
  cmake ? nixpkgs.cmake,
  nixBoost ? nixpkgs.boost17x
}:
let
  version = "alfa";
  commit = "46d4561c20";
            
  boost = nixBoost.override{ enableShared = true; enableStatic = true; };
  turtle = stdenv.mkDerivation {
    name = "turtle";
    nativeBuildInputs = [ cmake ];
    buildInputs = [ boost ];
    src = builtins.fetchGit {
      url = "https://github.com/mat007/turtle.git";
      name = "turtle.git";
      rev = "bfd1701fcbbb77258ce82dec5a755ff969cbadd3";
    };
  };
in stdenv.mkDerivation {
  name = "cogen";
  src = builtins.fetchTarball{
    url = "http://cpphttpx.org/repos/cogen/tarball/" + commit + "/cogen-" + commit + ".tar.gz";
    sha256 = "0lxzlnwlb70qhpi1mg65xkc6hhg57rj5ipkx7j3ch8qqv0j1saim";
  };
  CTEST_OUTPUT_ON_FAILURE=1;
  PYTHONDONTWRITEBYTECODE=1;
  nativeBuildInputs = [ cmake ];
  buildInputs = [ boost turtle ];
  cmakeFlags = ["-DCMAKE_BUILD_TYPE=Release"];
}
