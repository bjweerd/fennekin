bin_PROGRAMS = fennekin fconv

fennekin_SOURCES = main.cc datadir.h datadir.cc fennekin_tree.h fennekin_tree.cc xmlstring.hpp

fconv_SOURCES = fconv.cc fennekin_tree.h fennekin_tree.cc xmlstring.hpp

if PLATFORM_WINDOWS
  fennekin_SOURCES += fennekin.rc
  fennekin_LDFLAGS = -mwindows
endif

fennekin_CPPFLAGS = ${GTK_CFLAGS} ${WEBKITGTK_CFLAGS} ${LIBXML2_CFLAGS} -DDATADIR=\"$(pkgdatadir)\"
fennekin_LDADD = ${GTK_LIBS} ${WEBKITGTK_LIBS} ${LIBXML2_LIBS}


fconv_CPPFLAGS = ${LIBXML2_CFLAGS}
fconv_LDADD = ${LIBXML2_LIBS}

LIBS = 

pkgdata_DATA = fennekin.png fennekin.glade Universal.fennekin Languages.fennekin
EXTRA_DIST = $(pkgdata_DATA) fennekin.ico

.rc.o:
	$(RC) -i $< $@
