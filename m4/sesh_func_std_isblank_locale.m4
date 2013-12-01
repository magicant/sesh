# Copyright (C) 2013 WATANABE Yuki

# This sesh_func_std_isblank_locale.m4 is free software; the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY, to the extent permitted by law; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# serial 1

# SYNOPSIS
#
#   SESH_FUNC_STD_ISBLANK_LOCALE()
#
# DESCRIPTION
#
#   Check for the std::isblank(ChatT, std::locale) function template.

AC_DEFUN([SESH_FUNC_STD_ISBLANK_LOCALE], [
  AC_CACHE_CHECK(
    [for std::isblank(ChatT, locale)],
    [sesh_cv_func_std_isblank_locale],
    [AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
        [[#include <locale>]],
        [[return std::isblank(L'a', std::locale()) ? 1 : 0;]])],
      AS_VAR_SET([sesh_cv_func_std_isblank_locale], [[yes]]),
      AS_VAR_SET([sesh_cv_func_std_isblank_locale], [[no]]))])
  AS_VAR_IF([sesh_cv_func_std_isblank_locale], [[yes]],
    [AC_DEFINE([HAVE_STD_ISBLANK_LOCALE], [1],
      [std::isblank(CharT, std::locale) availability.])])
])
