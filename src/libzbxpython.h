#ifndef LIBZBXPYTHON_H
#define LIBZBXPYTHON_H

#include <stdio.h>

// Zabbix source headers
#define HAVE_TIME_H 1
#include <sysinc.h>
#include <module.h>
#include <common.h>
#include <log.h>
#include <zbxjson.h>
#include <version.h>

// Python headers
#include <Python.h>

#define PYTHON_MODULE "zabbix_module"

typedef struct
{
  char     *key;
  PyObject *function;
}
ZBX_METRIC_MAPPING;

pid_t       init_pid;

PyObject    *pySysModule;
PyObject    *pyAgentModule;
PyObject    *pyRouterFunc;

PyMODINIT_FUNC PyInit_zabbix_runtime(void);

int         python_log_error(AGENT_RESULT *result);
int         python_module_init(PyObject *pyModule);
ZBX_METRIC  *python_module_item_list(PyObject *pyModule);
char        *python_str(PyObject *pyValue);
PyObject    *python_import_module(const char *module);

int         PYTHON_ROUTER(AGENT_REQUEST *request, AGENT_RESULT *result);

#endif
