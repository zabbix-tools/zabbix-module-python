lib_LTLIBRARIES = libzbxpython.la

libzbxpython_la_SOURCES = \
	libzbxpython.h \
	libzbxpython.c \
	log.c \
	python.c \
	route.c \
	runtime.c

libzbxpython_la_CFLAGS = \
	$(DEBUG_CPPFLAGS) \
	$(ZABBIX_CPPFLAGS) \
	$(PYTHON_CPPFLAGS)

libzbxpython_la_LDFLAGS = \
	-shared \
	-avoid-version \
	$(PYTHON_EXTRA_LDFLAGS) \
	$(PYTHON_LIBS) \
	$(PYTHON_EXTRA_LIBS)

# Prevent install of the redundant *.la files
install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/libzbxpython.la

# Fix "files left after uninstall" issue
uninstall-local:
	rm -f $(DESTDIR)$(libdir)/libzbxpython.so
