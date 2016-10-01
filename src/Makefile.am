lib_LTLIBRARIES = python.la

python_la_SOURCES = \
	libzbxpython.h \
	libzbxpython.c \
	python.c \
	route.c \
	runtime.c

python_la_CFLAGS = \
	$(DEBUG_CPPFLAGS) \
	$(ZABBIX_CPPFLAGS) \
	$(PYTHON_CPPFLAGS)

python_la_LDFLAGS = \
	-shared \
	-module \
	-avoid-version \
	$(PYTHON_LIBS)

# prevent install of the redundant *.la files
# and rename module to include python version
install-exec-hook: install-agent-config
	rm -f $(DESTDIR)$(libdir)/python.la
	mv -f $(DESTDIR)$(libdir)/python.so \
		$(DESTDIR)$(libdir)/python$(PYTHON_VERSION).so

# install agent module configuration file
install-agent-config:
	$(MKDIR_P) $(DESTDIR)$(ZABBIX_CONFIG_PATH)/zabbix_agentd.d
	echo "LoadModule=python$(PYTHON_VERSION).so" > \
		$(DESTDIR)$(ZABBIX_CONFIG_PATH)/zabbix_agentd.d/python$(PYTHON_VERSION).conf
