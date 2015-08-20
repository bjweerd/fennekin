bin_PROGRAMS = fennekin fconv xmldump

fennekin_SOURCES = main.cc datadir.h datadir.cc document.h document.cc application.h application.cc \
	main_window.h main_window.cc fennekin_treemodel.h fennekin_treemodel.cc

fconv_SOURCES = fconv.cc xmlstring.hpp
xmldump_SOURCES = xmldump.cc xmlstring.hpp

if PLATFORM_WINDOWS
  fennekin_SOURCES += fennekin.rc
  fennekin_LDFLAGS = -mwindows
endif

fennekin_CPPFLAGS = ${GTK_CFLAGS} ${WEBKITGTK_CFLAGS} ${LIBXML2_CFLAGS} -DDATADIR=\"$(pkgdatadir)\"
fennekin_LDADD = ${GTK_LIBS} ${WEBKITGTK_LIBS} ${LIBXML2_LIBS}


fconv_CPPFLAGS = ${LIBXML2_CFLAGS}
fconv_LDADD = ${LIBXML2_LIBS}
xmldump_CPPFLAGS = ${LIBXML2_CFLAGS}
xmldump_LDADD = ${LIBXML2_LIBS}

LIBS = 

pkgdata_DATA = Fennekin.png Fennekin.ui Universal.fennekin Languages.fennekin
EXTRA_DIST = $(pkgdata_DATA) Fennekin.glade Fennekin.ico

Fennekin.ui : Fennekin.glade
	gtk-builder-convert Fennekin.glade Fennekin.ui

CLEANFILES = Fennekin.ui

.rc.o:
	$(RC) -i $< $@
