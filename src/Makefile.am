## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@ @X_CFLAGS@ @RENDER_CFLAGS@

bin_PROGRAMS = backstep

backstep_SOURCES = \
	glade_interface.c glade_interface.h \
	glade_callbacks.c glade_callbacks.h \
	support.c support.h \
	atoms.c atoms.h \
	backstep.c \
	callback.c callback.h \
	callbacklist.c callbacklist.h \
	callbacktable.c callbacktable.h \
	colors.c colors.h \
	composite.c composite.h \
	configuration.c configuration.h \
	configurator.c configurator.h \
	daemon.c daemon.h \
	damage.c damage.h \
	error.c error.h \
	groups.c groups.h \
	iconwindow.c iconwindow.h \
	managedwindow.c managedwindow.h \
	program.c program.h \
	session.c session.h \
	sessionclient.c sessionclient.h \
	settings.c settings.h \
	trace.h \
	window.c window.h \
	windowpixmap.c windowpixmap.h \
	windowpixmapicon.c windowpixmapicon.h \
	windowpixmaprender.c windowpixmaprender.h \
	windows.c windows.h \
	workspaces.c workspaces.h \
	xutil.c xutil.h

backstep_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) @X_LIBS@ @X_EXTRA_LIBS@ @RENDER_LIBS@

