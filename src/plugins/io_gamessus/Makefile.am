# Set plugin installation dir and define plugin targets
pluginexecdir = @ATEN_PLUGINLIBDIR@
pluginexec_LTLIBRARIES = gamessus.la gamessuslog.la

# Rules
.ui.lo:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $*.h
	${LIBTOOL} --tag=CXX --mode=compile ${CXX} -I$(top_srcdir)/src -I../ -I./ ${AM_CPPFLAGS} -c moc_$*.cpp -o $@
	rm moc_$*.cpp

.hui.lo:
	${QTMOC} -o $*.cpp -I../../ @ATEN_INCLUDES@ $<
	${LIBTOOL} --tag=CXX --mode=compile $(CXX) -I$(top_srcdir)/src -I../ -I./ ${AM_CPPFLAGS} -c $*.cpp -o $@
	rm $*.cpp

# Local clean (temporary files generated from rules)
clean-local:
	-rm -f gamessus.cpp gamessuslog.cpp

# GAMESS-US Input Model Plugin
gamessus_la_SOURCES = gamessusexportoptions.ui gamessusexportoptions_funcs.cpp gamessus.hui gamessus_funcs.cpp
gamessus_la_LDFLAGS = -module -shared -avoid-version

# GAMESS-US Logfile Model Plugin
gamessuslog_la_SOURCES = gamessuslog.hui gamessuslog_funcs.cpp
gamessuslog_la_LDFLAGS = -module -shared -avoid-version

AM_CPPFLAGS = -I${top_srcdir}/src @ATEN_INCLUDES@ @ATEN_CFLAGS@

noinst_HEADERS = gamessusexportoptions.h
