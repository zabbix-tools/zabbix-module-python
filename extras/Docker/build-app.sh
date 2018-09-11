#!/bin/bash

echo "************************************************************************"
echo ZBX_SOURCE_VERSION = ${ZBX_SOURCE_VERSION}
echo "--libdir=/usr/src/build"
echo "--with-zabbix=/usr/src/zabbix-"${ZBX_SOURCE_VERSION}
echo "--with-zabbix-conf=/etc/zabbix"
echo "************************************************************************"

rm -rf /usr/src/zabbix-module-python
cp -rf /root/app /usr/src/zabbix-module-python



if [ ! -d "/usr/src/zabbix-${ZBX_SOURCE_VERSION}" ]; then
  cd /usr/src && \
  wget https://sourceforge.net/projects/zabbix/files/ZABBIX%20Latest%20Stable/${ZBX_SOURCE_VERSION}/zabbix-${ZBX_SOURCE_VERSION}.tar.gz/download -O zabbix-${ZBX_SOURCE_VERSION}.tar.gz && \
  tar zxvf zabbix-${ZBX_SOURCE_VERSION}.tar.gz && \
  rm zabbix-${ZBX_SOURCE_VERSION}.tar.gz
fi

cd /usr/src/zabbix-module-python

autoreconf -ifv && \
PYTHON_VERSION=3 ./configure --libdir=/usr/lib --with-zabbix=/usr/src/zabbix-${ZBX_SOURCE_VERSION} --with-zabbix-conf=/etc/zabbix && \
make && \
make install

if [ ! -d "/root/app/build" ]; then
  mkdir /root/app/build
fi
cp /usr/lib/zabbix/modules/libzbxpython*.so /root/app/build/
