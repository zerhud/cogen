#!/usr/bin/env bash
cd ./build && ninja && ninja test && ./mdg2/mdgwr.py -iix3=ix3_example -gcpp_decl -o dir
