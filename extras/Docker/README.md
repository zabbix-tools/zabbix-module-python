# Build your the modules with Docker container

## As long as default container image is not available

```cd /extras/Docker
docker build . --rm --tag cavaliercoder/zmp:latest
```

## To build your module

If you build the image or an image is available just run the container.
You have to map the module root folder to `/root/app`.

```cd /zabbix-module-python
docker run --rm -v "$PWD":/root/app cavaliercoder/zmp:latest
```

The container will build the module and outputs the compiled files to $PWD/build afterwards.

## How?

Your source files aren't edited in any way. The container copies the /root/app mapped folder internaly of the container and compiles it there. Just the build modules are copied back after that.

## Modifications

The image is initially build with an provided version of the Zabbix Source directly from the official download sites. But you may alter it on container run with an environment modifier for the version.

```cd /zabbix-module-python
docker run --rm -v "$PWD":/root/app -e ZBX_SOURCE_VERSION=3.4.11 cavaliercoder/zmp:latest
```

> If you provide a version other than the baked in, it will be fetched while build time!