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
AM_PROG_CC_C_O

AC_ARG_ENABLE([debug-build],
    [AS_HELP_STRING(
        [--enable-debug-build], [change build options for debugging Sesh])],
    [],
    [enable_debug_build=no])

AC_LANG([C])
AS_VAR_IF([enable_debug_build], [[yes]], [[: ${CFLAGS=-g}]])
AC_PROG_CC_C99
AC_SYS_LARGEFILE
AS_VAR_IF([enable_debug_build], [[yes]], [AX_APPEND_COMPILE_FLAGS(
    [-pedantic -Wall -Wextra -Wunreachable-code -Wdocumentation -Werror])])

AC_LANG([C++])
AS_VAR_IF([enable_debug_build], [[yes]], [[: ${CXXFLAGS=-g}]])
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])
AS_VAR_IF([enable_debug_build], [[yes]], [AX_APPEND_COMPILE_FLAGS(
    [-pedantic -Wall -Wextra -Wunreachable-code -Wdocumentation])])
AX_GCC_BUILTIN([__builtin_unreachable]) # must be tested w/o -Werror
AS_VAR_IF([enable_debug_build], [[yes]], [AX_APPEND_COMPILE_FLAGS([-Werror])])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
