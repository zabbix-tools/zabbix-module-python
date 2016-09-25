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

PyObject    *pySysModule;
PyObject    *pyAgentModule;

int         verrorf(AGENT_RESULT *result, const char *format, va_list args);
int         errorf(AGENT_RESULT *result, const char *format, ...);
int         perrorf(AGENT_RESULT *result, const char *format, ...);

ZBX_METRIC  *get_module_item_list(AGENT_RESULT *result, PyObject *pyModule);

int         python_add_module_path(AGENT_RESULT *result, const char *path);
PyObject    *python_import_module(AGENT_RESULT *result, const char *module);

int         PYTHON_MODVER(AGENT_REQUEST *request, AGENT_RESULT *result);
int         PYTHON_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result);

#endif
