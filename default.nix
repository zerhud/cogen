{ nixpkgs ? import<nixpkgs>{},
  stdenv ? nixpkgs.gcc10Stdenv,
  cmake ? nixpkgs.cmake,
  nixBoost ? nixpkgs.boost17x
}:
let
  version = "alfa";
  commit = "9c563ca825";
            
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
    sha256 = "08963xcf7iyhmgzw0bjc8kay6f1b4m9b3gbliyf0vciq6b7xc0bl";
  };
  CTEST_OUTPUT_ON_FAILURE=1;
  PYTHONDONTWRITEBYTECODE=1;
  nativeBuildInputs = [ cmake ];
  buildInputs = [ boost turtle ];
  cmakeFlags = ["-DCMAKE_BUILD_TYPE=Release"];
}
