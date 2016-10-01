# libzbxpython

Embedded Python interpreter module for Zabbix.

This [native Zabbix module](https://www.zabbix.com/documentation/3.2/manual/config/items/loadablemodules)
allows you to write extensions for Zabbix in Python and run them embedded in the
Zabbix agent, server or proxy.

Why bother?

* Extensions are much simpler to write and manage.
  
  The following is a working Zabbix module written in Python. It creates item
  key `python.echo[]` which simply returns a string concatenation of each given
  parameter:

  ```python
  import zabbix_module

  def echo(request):
    return " ".join(request.params)

  def zbx_module_item_list():
    return [
      zabbix_module.AgentItem("python.echo", fn = echo, test_param = [ 'hello', 'world' ]),
    ]

  ```

  It works like this:

  ```
  $ zabbix_agentd -t python.echo[hello,world]
  zabbix_agentd [19]: loaded python modules: dummy.py
  python.echo[hello,world]                      [s|hello world]
  ```

* The embedded interpreter outperforms User Parameter scripts by an order of
  magnitude with a lower memory footprint

* Maintaining state between requests, working with threads or watching resources
  is much simpler in a long lived, shared library context than in on-demand
  script calls


## Requirements

This project is immature and pre-release. 

For now, testing has only been completed with the following prerequisites:

* Debian Jessie
* Zabbix Agent v3.2
* Python v3.4


## Installation

This project is immature and pre-release. 

Download [source tarball here](http://s3.cavaliercoder.com/libzbxpython/libzbxpython-1.0.0.tar.gz).

* Configure module sources with the desired Python version, the location of
  Zabbix sources and the install location of the Zabbix configuration directory

  ```
  $ PYTHON_VERSION=3 ./configure \
                        --libdir=/usr/lib/zabbix/modules \
                        --with-zabbix=/usr/src/zabbix-3.2.0 \
                        --with-zabbix-conf=/etc/zabbix
  ```

* Compile the module binary
  
  ```
  $ make
  ```

* Install the Zabbix and Python modules
  
  ```
  $ make install
  ```

## Usage

See `dummy.py` for a fully functioning example.

* Python modules need to be installed in a `python` subdirectory of the Zabbix
  module directory (default: `/usr/lib/zabbix/modules/python3`)

* Start your Python module with `import zabbix_module`

* If you need to initialize any globals or threads before Zabbix forks, you may
  do this by implementing `zbx_module_init`:

  ```python
  def zbx_module_init():
    zabbix_module.info("Initalized my module")
  ```

* Create handler functions for each item you wish to create. These funtions
  should accept a single `request` parameter and must return either a positive
  integer, a double or a string with less than 255 characters

  ```python
  def my_handler(request):
    return 'hello world'
  ```

  The `request` parameter is an `AgentRequest` object and exposes the requested
  key and parameters.

* Register your item handlers by implementing `zbx_module_item_list`

  ```python
  def zbx_module_item_list():
    return [
      zabbix_module.AgentItem("my.item", fn = my_handler)
    ]
  ```

* For discovery rules, convert a `dict` into a JSON discovery string using
  the `discovery` function:

  ```python
  def my_handler(request):
    return zabbix_module.discovery( [ { 'id': 1 }, { 'id': 2 } ] )
  ```

* To send an error to Zabbix, simply raise an exception:
  
  ```python
  def my_handler(request):
    raise ValueError('Something went wrong')
  ```

* You can log messages to the Zabbix log file using any of the following
  functions: `trace`, `debug`, `info`, `warning`, `error` or `critical`

  ```python
  def my_hander(request):
    zabbix_modue.info('Received request %s[%s]' % request.key, ','.join(request.params))

    return 1

  ```

## License

Copyright (c) 2016 Ryan Armstrong

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
