set -eux

export CXXFLAGS=-g

autoreconf -i
sh configure
make distcheck
