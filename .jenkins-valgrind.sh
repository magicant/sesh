set -eu

export CXXFLAGS='-g'

for tarball in *-*.tar.gz
do
  gzip -dc "$tarball" | tar -x -f -
done
cd *-*/

sh configure
trap 'cat src/test-suite.log' EXIT
make -k LOG_COMPILER='$(SHELL) $(top_srcdir)/../.jenkins-log.sh' check
