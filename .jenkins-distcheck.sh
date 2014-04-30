set -eu

for tarball in *-*.tar.gz
do
  gzip -dc "$tarball" | tar -x -f -
done
cd *-*/

sh configure
make -k VERBOSE=1 distcheck
