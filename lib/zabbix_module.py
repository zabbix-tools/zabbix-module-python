import sys
import glob
import os.path

__version__           = "1.0.0"

python_version_string = "Python %i.%i.%i-%s" % sys.version_info[:4]

zabbix_module_path    = "/var/lib/zabbix/modules/python"

routes                = {}

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

  if not routes[request.key]:
    raise ValueException("no function mapped for key " + request.key)

  return routes[request.key](request)

def version(request):
  """Agent item python.version returns the runtime version string"""
  
  return python_version_string

def zbx_module_item_list():
  # add builtin items
  items = [
    AgentItem("python.version", fn = version),
  ]

  # ensure module path is in search path
  sys.path.insert(0, zabbix_module_path)

  # iterate over installed modules
  for path in glob.glob(zabbix_module_path + "/*.py"):
    mod_name = os.path.basename(path)
    mod_name = mod_name[0:len(mod_name) - 3]

    # skip self if it's in the same directory
    if mod_name == "zabbix_module":
      continue

    # import module
    mod = __import__(mod_name)
    items += mod.zbx_module_item_list()

  # add routes for imported module items
  for item in items:
    routes[item.key] = item.fn

  return items
