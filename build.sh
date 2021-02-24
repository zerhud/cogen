#!/usr/bin/env bash
cd ./build && ninja && ninja test && ./mdg2/mdgwr.py -iix3=ix3_example -I mdg2/etc/mdg2/examples/incs -gcpp_py -o dir
