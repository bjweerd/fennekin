AC_INIT([fennekin], [0.3.0], [kamintears@gmail.com], [], [http://www.github.com/kamintears/fennekin/])
AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_MACRO_DIR([m4])
AC_PREREQ(2.59)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS(config.h)

AC_PROG_CXX

PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.14.0])
PKG_CHECK_MODULES([GIO], [gio-2.0 >= 2.28.0])
PKG_CHECK_MODULES([LIBXML2], [libxml-2.0 >= 2.7.6])
PKG_CHECK_MODULES([WEBKITGTK], [webkitgtk-3.0 >= 2.2.8])


# check for windows
AC_CANONICAL_HOST
AC_MSG_CHECKING([for some Win32 platform])
case "$host" in
  *mingw*|*cygwin*)
    platform_win32=yes
    if test -z $host_alias; then
        RC=windres
    else
        RC=$host_alias-windres
    fi
    AC_SUBST(RC)
    ;;
  *)
    platform_win32=no
    ;;
esac
AC_MSG_RESULT([$platform_win32])
AM_CONDITIONAL(PLATFORM_WINDOWS, test "$platform_win32" = "yes")

AC_CONFIG_FILES(Makefile src/Makefile fennekin.nsi)
AC_OUTPUT
