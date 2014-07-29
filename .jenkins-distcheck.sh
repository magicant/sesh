set -eu

autoreconf -i
sh configure
make -k VERBOSE=1 distcheck
