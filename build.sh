#!/usr/bin/env bash
cd ./build && ninja && ninja test
./cogen/cogenwr.py -iix3=ix3_example -I cogen/etc/cogen/examples/incs -gcpp_py -o dir
./cogen/cogenwr.py -iix3=ix3_example -I cogen/etc/cogen/examples/incs -gcpp_decl -o dir_decl
