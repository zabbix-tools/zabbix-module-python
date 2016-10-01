all: build-zabbix-module

clean-local: clean-zabbix-module

install-exec-local: install-zabbix-module install-dummy-module

build-zabbix-module:
	$(PYTHON) setup.py build -v

clean-zabbix-module:
	rm -rf build/ dist/ zabbix_module.egg-info/

install-zabbix-module:
	$(PYTHON) setup.py install \
		--install-lib=$(PYTHON_SITE_PKG)

install-dummy-module:
	$(MKDIR_P) $(ZABBIX_PYTHON_MODULE_PATH)
	$(INSTALL_DATA) dummy.py $(ZABBIX_PYTHON_MODULE_PATH)/dummy.py