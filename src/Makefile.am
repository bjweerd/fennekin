bin_PROGRAMS = fennekin
fennekin_SOURCES = main.cc datadir.h datadir.cc document.h document.cc application.h application.cc \
	main_window.h main_window.cc

if PLATFORM_WINDOWS
  fennekin_SOURCES += fennekin.rc
  fennekin_LDFLAGS = -mwindows
endif

fennekin_CPPFLAGS = ${GTK_CFLAGS} ${GIO_CFLAGS} ${WEBKIT_CFLAGS} ${LIBXML2_CFLAGS} -DDATADIR=\"$(pkgdatadir)\"
fennekin_LDADD = ${GTK_LIBS} ${GIO_LIBS} ${WEBKIT_LIBS} ${LIBXML2_LIBS}

LIBS = 

pkgdata_DATA = Fennekin.png Fennekin.ui Universal.fennekin Languages.fennekin
EXTRA_DIST = $(pkgdata_DATA) Fennekin.glade Fennekin.ico

Fennekin.ui : Fennekin.glade
	gtk-builder-convert Fennekin.glade Fennekin.ui

CLEANFILES = Fennekin.ui

.rc.o:
	windres -i $< $@
