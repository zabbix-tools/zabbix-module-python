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
