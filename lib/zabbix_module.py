import sys
import glob
import os.path

__version__           = "1.0.0"

__python_version_string = "Python %i.%i.%i-%s" % sys.version_info[:4]

__modules             = []

__items               = []

__routes              = {}

zabbix_module_path    = "/var/lib/zabbix/modules/python"

item_timeout          = 0

class AgentRequest:
  key    = None
  params = []

  def __init__(self, key, params):
    self.key = key
    self.params = params

class AgentItem:
  key        = None
  flags      = 0
  test_param = None
  fn         = None

  def __init__(self, key, flags = 0, fn = None, test_param = None):
    if not key:
      raise ValueError("key not given in agent item")

    if not fn:
      raise ValueError("fn not given in agent item")

    # join test_param if list or tuple given
    if test_param:        
      try:
        for i, v in enumerate(test_param):
          test_param[i] = str(v)

        test_param = ",".join(test_param)
      except TypeError:
        test_param = str(test_param)

    self.key = key
    self.flags = flags
    self.test_param = test_param
    self.fn = fn

  def __str__(self):
    return self.key

def route(request):
  """
  Route a request from the Zabbix agent to the Python function associated with
  the request key.
  """

  if not __routes[request.key]:
    raise ValueException("no function mapped for key " + request.key)

  return __routes[request.key](request)

def version(request):
  """Agent item python.version returns the runtime version string"""
  
  return python_version_string

def register_item(item):
  __items.append(item)
  __routes[item.key] = item.fn

  return item

def register_module_items(mod):
  try:
    newitems = mod.zbx_module_item_list()
    for item in newitems:
      register_item(item)

  except AttributeError:
    pass

  return __items

def register_module(mod_name):
  # import module
  mod = __import__(mod_name)
  __modules.append(mod)

  # init module
  try:
    mod.zbx_module_init()
  except AttributeError:
    pass

  # register items
  register_module_items(mod)

  return mod

def zbx_module_init():
  # ensure module path is in search path
  sys.path.insert(0, zabbix_module_path)

  # register builtin items
  register_item(AgentItem("python.version", fn = version))

  # register installed agent modules
  for path in glob.glob(zabbix_module_path + "/*.py"):
    mod_name = os.path.basename(path)
    mod_name = mod_name[0:len(mod_name) - 3]

    # skip self if it's in the same directory
    if mod_name == "zabbix_module":
      continue

    register_module(mod_name)

def zbx_module_item_list():
  return __items
