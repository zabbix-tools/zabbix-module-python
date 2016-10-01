"""dummy module for the Zabbix agent"""

import zabbix_module
import random

def ping(request):
  """agent item python.ping[]"""

  if len(request.params) != 0:
    raise ValueError('Invalid number of parameters')

  return 1

def echo(request):
  """agent item python.echo[msg, ...]"""

  if len(request.params) == 0:
    raise ValueError('Invalid number of parameters')

  return " ".join(request.params)

def rand(request):
  """agent item python.random[lower, upper]"""

  if len(request.params) != 2:
    raise ValueError('Invalid number of parameters')

  l = int(request.params[0], 10)
  u = int(request.params[1], 10)
  
  if l > u:
    raise ValueError('Incorrect range given')

  return random.randrange(l, u)

def discovery(request):
  """agent item python.discovery[...]"""

  data = []
  for param in request.params:
    data.append({ 'param': param })

  return zabbix_module.discovery(data)

def zbx_module_init():
  """initialize this module"""

  zabbix_module.info("Initalized dummy module")
  random.seed()

def zbx_module_item_list():
  """register available module items"""

  return [
    zabbix_module.AgentItem("python.ping",      fn = ping),
    zabbix_module.AgentItem("python.echo",      fn = echo,      test_param = [ 'hello', 'world' ]),
    zabbix_module.AgentItem("python.random",    fn = rand,      test_param = [ 1, 1000 ]),
    zabbix_module.AgentItem("python.discovery", fn = discovery, test_param = [ 'lorem', 'ipsum', 'dolar' ]),
  ]
