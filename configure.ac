AC_INIT([fennekin], [0.2.0], [kamintears@gmail.com], [], [http://www.github.com/kamintears/fennekin/])
AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_MACRO_DIR([m4])
AC_PREREQ(2.59)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS(config.h)

AC_PROG_CXX

PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.24.0])
PKG_CHECK_MODULES([GIO], [gio-2.0 >= 2.36.0])
PKG_CHECK_MODULES([WEBKIT], [webkit-1.0 >= 1.8.3])
PKG_CHECK_MODULES([LIBXML2], [libxml-2.0 >= 2.8.0]);

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT
