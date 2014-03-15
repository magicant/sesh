# Copyright (C) 2013 WATANABE Yuki

# This configure.ac is free software; the author gives unlimited permission to
# copy and/or distribute it, with or without modifications, as long as this
# notice is preserved.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY, to the extent permitted by law; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_PREREQ([2.68])
AC_INIT([Sesh], [0], [magicant@wonderwand.net], [sesh],
    [https://github.com/magicant/sesh])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([
    1.11
    dist-xz
    foreign
    no-installinfo
    parallel-tests
    subdir-objects
    tar-ustar
    -Wall
    -Werror
])

AC_ARG_ENABLE([debug-build],
    [AS_HELP_STRING(
        [--enable-debug-build], [change build options for debugging Sesh])],
    [],
    [enable_debug_build=no])

AC_LANG([C++])
AS_VAR_IF([enable_debug_build], [[yes]], [[: ${CXXFLAGS=-g}]])
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])
AS_VAR_IF([enable_debug_build], [[yes]],
    [AX_APPEND_COMPILE_FLAGS([-pedantic -Wall -Wextra -Wdocumentation])])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
])
AC_OUTPUT
