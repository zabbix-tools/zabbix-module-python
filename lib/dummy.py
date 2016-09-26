import zabbix_module
import random

def ping(request):
  return 1

def echo(request):
  return " ".join(request.params)

def rand(request):
  if len(request.params) != 2:
    raise ValueError('Invalid number of parameters')

  l = int(request.params[0], 10)
  u = int(request.params[1], 10)
  
  if l > u:
    raise ValueError('Incorrect range given')

  return random.randrange(l, u)

def zbx_module_item_list():
  return [
    zabbix_module.AgentItem("python.ping",   fn = ping),
    zabbix_module.AgentItem("python.echo",   fn = echo, test_param = [ 'hello', 'world' ]),
    zabbix_module.AgentItem("python.random", fn = rand, test_param = [ 1, 1000 ]),
  ]
