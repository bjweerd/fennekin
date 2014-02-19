bin_PROGRAMS = fennekin
fennekin_SOURCES = main.cc

fennekin_CPPFLAGS = ${GTK_CFLAGS} ${GIO_CFLAGS} ${WEBKIT_CFLAGS} ${LIBXML2_CFLAGS} -DDATADIR=\"$(pkgdatadir)\"
fennekin_LDADD = ${GTK_LIBS} ${GIO_LIBS} ${WEBKIT_LIBS} ${LIBXML2_LIBS}

LIBS = 

pkgdata_DATA = Fennekin.png Fennekin.ui
EXTRA_DIST = $(pkgdata_DATA) Fennekin.glade

Fennekin.ui : Fennekin.glade
	gtk-builder-convert Fennekin.glade Fennekin.ui

CLEANFILES = Fennekin.ui
