bin_PROGRAMS = fennekin
fennekin_SOURCES = main.cc document.h document.cc

if PLATFORM_WINDOWS
  $(name)_SOURCES += fennekin.rc
  $(name)_LDFLAGS = -mwindows
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
