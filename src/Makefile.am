bin_PROGRAMS = fennekingtk2
fennekingtk2_SOURCES = main.cc datadir.h datadir.cc document.h document.cc application.h application.cc \
	main_window.h main_window.cc fennekin_treemodel.h fennekin_treemodel.cc

if PLATFORM_WINDOWS
  fennekingtk2_SOURCES += fennekin.rc
  fennekingtk2_LDFLAGS = -mwindows
endif

fennekingtk2_CPPFLAGS = ${GTK_CFLAGS} ${GIO_CFLAGS} ${WEBKIT1_CFLAGS} ${LIBXML2_CFLAGS} -DDATADIR=\"$(pkgdatadir)\"
fennekingtk2_LDADD = ${GTK_LIBS} ${GIO_LIBS} ${WEBKIT1_LIBS} ${LIBXML2_LIBS}

LIBS = 

pkgdata_DATA = Fennekin.png Fennekin.ui Universal.fennekin Languages.fennekin
EXTRA_DIST = $(pkgdata_DATA) Fennekin.glade Fennekin.ico

Fennekin.ui : Fennekin.glade
	gtk-builder-convert Fennekin.glade Fennekin.ui

CLEANFILES = Fennekin.ui

.rc.o:
	$(RC) -i $< $@
