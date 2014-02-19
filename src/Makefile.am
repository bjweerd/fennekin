bin_PROGRAMS = fennekin
fennekin_SOURCES = main.cc

fennekin_CPPFLAGS = ${GTK_CFLAGS} ${WEBKIT_CFLAGS} ${LIBXML2_CFLAGS} $(BOOST_CPPFLAGS) $(SQLITE3_CFLAGS) -DDATADIR=\"$(pkgdatadir)\"
fennekin_LDADD = ${GTK_LIBS} ${WEBKIT_LIBS} ${LIBXML2_LIBS} $(BOOST_LDFLAGS) $(SQLITE3_LDFLAGS)

LIBS = $(BOOST_SYSTEM_LIB) $(BOOST_ASIO_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)
pkgdata_DATA = Fennekin.png Fennekin.ui
EXTRA_DIST = $(pkgdata_DATA) Fennekin.glade

Fennekin.ui : Fennekin.glade
	gtk-builder-convert Fennekin.glade Fennekin.ui

