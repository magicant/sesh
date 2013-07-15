AC_PREREQ([2.69])
AC_INIT([Sesh], [0], [magicant@wonderwand.net], [sesh],
    [https://github.com/magicant/sesh])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([
    1.13
    dist-xz
    foreign
    no-installinfo
    subdir-objects
    -Wall
    -Werror
])

AC_LANG([C++])
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
])
AC_OUTPUT
