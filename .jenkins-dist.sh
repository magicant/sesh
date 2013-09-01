set -eu

autoreconf -i
sh configure
make dist-gzip
