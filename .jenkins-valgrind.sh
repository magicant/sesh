set -eu

autoreconf -i
sh configure --disable-dependency-tracking --enable-debug-build
make -k LOG_COMPILER='$(SHELL) $(top_srcdir)/.jenkins-log.sh' VERBOSE=1 check
