import sys

__version__ = "1.0.0"

python_version_string = "Python %i.%i.%i-%s" % sys.version_info[:4]

class AgentRequest:
  key = ""
  params = []

def echo(request):
  print ("%s\n" % request)

keys = {
  "python.echo": echo
}
