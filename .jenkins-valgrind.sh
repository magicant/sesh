set -eu

for tarball in *-*.tar.gz
do
  gzip -dc "$tarball" | tar -x -f -
done
cd *-*/

sh configure --disable-dependency-tracking --enable-debug-build
trap 'cat test-suite.log' EXIT
make -k LOG_COMPILER='$(SHELL) $(top_srcdir)/../.jenkins-log.sh' check
