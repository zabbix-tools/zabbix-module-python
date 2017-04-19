lib_LTLIBRARIES = libzbxpython.la

libzbxpython_la_SOURCES = \
	libzbxpython.h \
	libzbxpython.c \
	python.c \
	route.c \
	runtime.c

libzbxpython_la_CFLAGS = \
	$(DEBUG_CPPFLAGS) \
	$(ZABBIX_CPPFLAGS) \
	$(PYTHON_CPPFLAGS)

libzbxpython_la_LDFLAGS = \
	-shared \
	-module \
	-avoid-version \
	$(PYTHON_LIBS)

install-exec-hook: install-module install-module-config

# prevent install of the redundant *.la files
# and rename module to include python version
install-module:
	$(MKDIR_P) $(DESTDIR)$(libdir)/zabbix/modules/
	rm -f $(DESTDIR)$(libdir)/libzbxpython.la
	mv -f $(DESTDIR)$(libdir)/libzbxpython.so \
		$(DESTDIR)$(libdir)/zabbix/modules/libzbxpython$(PYTHON_VERSION).so

install-module-config:
	$(MKDIR_P) $(DESTDIR)$(ZABBIX_CONFIG_PATH)/zabbix_agentd.d
	$(INSTALL) -m 0644 ../../../conf/libzbxpython$(PYTHON_VERSION).conf \
		$(DESTDIR)$(ZABBIX_CONFIG_PATH)/zabbix_agentd.d/libzbxpython$(PYTHON_VERSION).conf
