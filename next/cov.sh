#! /usr/bin/env bash

mkdir build-cov
cd build-cov

# -isystem $fs_inc_path
#fs_inc_path=`env | grep gcc-8 | grep '^PATH='| sed 's/PATH=//' | awk -F: '{for(i=0;i<NF;++i) print $i}'|sed 1d|grep gcc-8| sort | uniq | sed -r 's#([0-9.]+)/bin#\1/include/c++/\1/#'`

cmake -G Ninja \
	-DCMAKE_C_COMPILER=`which clang` -DCMAKE_CXX_COMPILER=`which clang++` \
	-DCMAKE_CXX_FLAGS="-fprofile-instr-generate -fcoverage-mapping " \
	-DCMAKE_C_FLAGS="-fprofile-instr-generate -fcoverage-mapping" \
	-DENABLE_TODO=OFF \
	..

cmake --build .
#LLVM_PROFILE_FILE="tests.profraw" cmake --build . --target test

cat > make_cov.sh <<EOF
#! /usr/bin/env bash

ninja

LLVM_PROFILE_FILE="\$1.profraw" ./\$1
llvm-profdata merge -sparse \$1.profraw -o \$1.profdata
llvm-cov show ./\$1 -show-line-counts-or-regions -instr-profile=\$1.profdata --format html > \$1.html
EOF

chmod +x make_cov.sh

