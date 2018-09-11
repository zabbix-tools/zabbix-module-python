FROM python:3.7-stretch

ARG BUILDIN_ZBX_SOURCE_VERSION=3.4.13

ENV ZBX_SOURCE_VERSION=${BUILDIN_ZBX_SOURCE_VERSION}

RUN cd /usr/src && \
    wget https://sourceforge.net/projects/zabbix/files/ZABBIX%20Latest%20Stable/${ZBX_SOURCE_VERSION}/zabbix-${ZBX_SOURCE_VERSION}.tar.gz/download -O zabbix-${ZBX_SOURCE_VERSION}.tar.gz && \
    tar zxvf zabbix-${ZBX_SOURCE_VERSION}.tar.gz && \
    rm zabbix-${ZBX_SOURCE_VERSION}.tar.gz

WORKDIR /root/app

COPY [ "build-app.sh", "/usr/bin" ]

RUN mkdir /etc/zabbix && \
    mkdir /usr/lib/zabbix && \
    mkdir /usr/lib/zabbix/modules && \
    mkdir /var/lib/zabbix && \
    mkdir /var/lib/zabbix/modules && \ 
    chmod 0777 /usr/bin/build-app.sh

VOLUME [ /root/app ]

CMD [ "/bin/bash" ]

ENTRYPOINT [ "/usr/bin/build-app.sh" ]
