FROM zabbix/zabbix-proxy-sqlite3:ubuntu-latest

ARG PYTHON_VERSION=3
ARG ZBX_SOURCE_VERSION=3.4.13

RUN apt-get update && apt-get install --no-install-recommends -y git gcc autoconf automake libtool gcc make unzip python3-dev && \
    apt-get install --no-install-recommends -y python3 wget ca-certificates jq && \
    apt-get autoremove && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN cd /usr/src && \
    wget https://sourceforge.net/projects/zabbix/files/ZABBIX%20Latest%20Stable/${ZBX_SOURCE_VERSION}/zabbix-${ZBX_SOURCE_VERSION}.tar.gz/download -O zabbix-${ZBX_SOURCE_VERSION}.tar.gz && \
    tar zxvf zabbix-${ZBX_SOURCE_VERSION}.tar.gz && \
    cd zabbix-${ZBX_SOURCE_VERSION}

RUN cd /usr/src && \
    git clone https://github.com/rockaut/zabbix-module-python.git && \
    cd zabbix-module-python

RUN cd /usr/src/zabbix-module-python && \
    ./autogen.sh && \
    PYTHON_VERSION=${PYTHON_VERSION} ./configure --libdir=/usr/lib --with-zabbix=/usr/src/zabbix-${ZBX_SOURCE_VERSION} --with-zabbix-conf=/etc/zabbix && \
    make && \
    make install

RUN cp /usr/lib/zabbix/modules/libzbxpython3.so /var/lib/zabbix/modules/ && \
    mkdir /var/lib/zabbix/modules/python3

RUN rm -rf /etc/zabbix/zabbix_agentd.d

COPY docker-entrypoint.sh /usr/local/bin/
RUN ln -s usr/local/bin/docker-entrypoint.sh / # backwards compat
ENTRYPOINT ["/docker-entrypoint.sh"]