#include "libzbxpython.h"

int python_add_module_path(AGENT_RESULT *result, const char *path)
{
    int ret = SYSINFO_RET_FAIL;
    PyObject *pyList, *pyFunc, *pyArgs;

    pyList = PyObject_GetAttrString(pySysModule, "path");
    if (NULL == pyList)
        return perrorf(result, "unable to read sys.path");

    pyFunc = PyObject_GetAttrString(pyList, "insert");
    if (NULL == pyFunc)
        return perrorf(result, "unable to find sys.path function");

    pyArgs = PyTuple_New(2);
    PyTuple_SetItem(pyArgs, 0, PyLong_FromLong(0));
    PyTuple_SetItem(pyArgs, 1, PyUnicode_FromString("."));

    PyObject_CallObject(pyFunc, pyArgs);
    
    if(NULL != PyErr_Occurred())
        perrorf(result, "unable to append path to sys.path");
    else
        ret = SYSINFO_RET_OK;

    Py_DECREF(pyArgs);
    Py_DECREF(pyFunc);
    Py_DECREF(pyList);

    return ret;
}

PyObject *python_import_module(AGENT_RESULT *result, const char *module)
{
    PyObject *pyName = NULL;
    PyObject *pyModule = NULL;

    pyName = PyUnicode_FromString(module);
    pyModule = PyImport_Import(pyName);
    Py_DECREF(pyName);

    if (NULL == pyModule)
        perrorf(result, "cannot import module %s", module);

    return pyModule;
}

ZBX_METRIC *get_module_item_list(AGENT_RESULT *result, PyObject *pyModule)
{
    ZBX_METRIC *keys = NULL;
    PyObject *pyKeys = NULL;

    const char *moduleName = PyModule_GetName(pyModule);

    // get keys attribute
    if(NULL == (pyKeys = PyObject_GetAttrString(pyModule, "keys"))) {
        perrorf(result, "cannot read item list in module %s", moduleName);
        return keys;
    } else {
        if (0 == PyList_Check(pyKeys)) {
            errorf(result, "keys attribute is not a python list in module %s", moduleName);
            return keys;
        } else {
            // expand keys
        }
    }

    return keys;
}

