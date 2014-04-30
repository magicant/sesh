set -eu

for tarball in *-*.tar.gz
do
  gzip -dc "$tarball" | tar -x -f -
done
cd *-*/

sh configure --disable-dependency-tracking --enable-debug-build
make -k LOG_COMPILER='$(SHELL) $(top_srcdir)/../.jenkins-log.sh' VERBOSE=1 \
  check
