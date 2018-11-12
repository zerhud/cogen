{  pkgs ? import <nixpkgs> {}
 , stdenv ? pkgs.stdenv
 , repository_url ? "http://cpphttpx.org/repos"
 , name
 , fossil_hash
 , sha256
}:
let info = rec {
	fossil_shash = stdenv.lib.substring 0 10 fossil_hash;
	tarball_name = "${name}-${fossil_shash}.tar.gz";
};
in with info; pkgs.fetchurl {
	url = "${repository_url}/${name}/tarball/${tarball_name}?uuid=${fossil_hash}";
	name = "${tarball_name}";
	inherit sha256;
}
