EXTRA_DIST = \
	dummy.py \
	zabbix_module.py

all:

install-exec-local: install-zabbix-module install-dummy-module

install-zabbix-module:
	$(MKDIR_P) $(DESTDIR)$(PYTHON_SITE_PKG)
	$(INSTALL_DATA) zabbix_module.py $(DESTDIR)$(PYTHON_SITE_PKG)/zabbix_module.py

install-dummy-module:
	$(MKDIR_P) $(DESTDIR)$(ZABBIX_PYTHON_MODULE_PATH)
	$(INSTALL_DATA) dummy.py $(DESTDIR)$(ZABBIX_PYTHON_MODULE_PATH)/dummy.py
