#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([editor4sh], [using-autotools], [https://github.com/polikuo/editor4sh])
AC_CONFIG_SRCDIR([editor4sh.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_PREFIX_DEFAULT([/usr/local])

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for libraries.
AC_SEARCH_LIBS([fl_system], [fltk], [], [
  AC_MSG_ERROR([Can not find 'fltk' library])
])

# Checks for header files.
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([ \
FL/Fl.H \
FL/Fl_Button.H \
FL/Fl_Double_Window.H \
FL/Fl_File_Chooser.H \
FL/Fl_Input.H \
FL/Fl_Menu_Bar.H \
FL/Fl_Return_Button.H \
FL/Fl_Text_Editor.H
])
AC_LANG_POP

# fltk-config
AC_CHECK_PROG(FC, [fltk-config], [yes], [no])
if test "$FC" = "no"; then
  AC_MSG_ERROR([Unable to find the fltk-config command.])
fi

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset select strerror])

# a hack for Makefile.am
AC_SUBST([DOLLAR_SIGN],[$])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
