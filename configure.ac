AC_INIT([fennekin], [0.3.0], [kamintears@gmail.com], [], [http://www.github.com/kamintears/fennekin/])
AC_CONFIG_SRCDIR(src/main.cc)
AC_CONFIG_MACRO_DIR([m4])
AC_PREREQ(2.59)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS(config.h)

AC_PROG_LIBTOOL

AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11

PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.8.0])
PKG_CHECK_MODULES([LIBXML2], [libxml-2.0 >= 2.7.6])
PKG_CHECK_MODULES([WEBKITGTK], [webkit2gtk-3.0 >= 2.0.4])


# check for windows
AC_CANONICAL_HOST
AC_MSG_CHECKING([for some Win32 platform])
case "$host" in
  *mingw*|*cygwin*)
    platform_win32=yes
    CPPFLAGS="$CPPFLAGS -D_WIN32_WINNT=0x0501 -DWINVER=0x0501"
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


BOOST_REQUIRE([1.42])
BOOST_SYSTEM
#BOOST_PROGRAM_OPTIONS
#BOOST_ASIO


AC_CONFIG_FILES(Makefile src/Makefile fennekin.nsi)
AC_OUTPUT
