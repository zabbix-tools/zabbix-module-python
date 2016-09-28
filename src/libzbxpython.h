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

void        vinfof(const char *format, va_list args);
void        infof(const char *format, ...);

void        vdebugf(const char *format, va_list args);
void        debugf(const char *format, ...);

int         verrorf(AGENT_RESULT *result, const char *format, va_list args);
int         errorf(AGENT_RESULT *result, const char *format, ...);
int         perrorf(AGENT_RESULT *result, const char *format, ...);

int         zbx_metric_len(ZBX_METRIC *m);
ZBX_METRIC  *zbx_metric_merge(ZBX_METRIC *a, ZBX_METRIC *b);

PyMODINIT_FUNC PyInit_zabbix_runtime(void);

int         python_module_init(PyObject *pyModule);
ZBX_METRIC  *python_module_item_list(PyObject *pyModule);
char        *python_str(PyObject *pyValue);
int         python_add_module_path(AGENT_RESULT *result, const char *path);
PyObject    *python_import_module(AGENT_RESULT *result, const char *module);

int         PYTHON_MARSHAL(AGENT_REQUEST *request, AGENT_RESULT *result);
int         PYTHON_MODVER(AGENT_REQUEST *request, AGENT_RESULT *result);

#endif
