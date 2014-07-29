set -eu

autoreconf -i
sh configure
make -k doxygen
