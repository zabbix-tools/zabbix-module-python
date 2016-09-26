#include "libzbxpython.h"

char *python_str(PyObject *pyValue)
{
    char *buf = NULL;
    PyObject *pyStr = NULL;

    if(NULL != (pyStr = PyObject_Str(pyValue))) {
        buf = strdup(PyUnicode_AsUTF8(pyStr));
        Py_DECREF(pyStr);
    }

    return buf;
}

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
    
    if(NULL != PyErr_Occurred()) {
        perrorf(result, "unable to append path to sys.path");
    } else {
        debugf("added directory to sys.path: %s", path);
        ret = SYSINFO_RET_OK;
    }

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

ZBX_METRIC *get_module_item_list(PyObject *pyModule)
{
    ZBX_METRIC *keys = NULL, *item;
    PyObject *pyFunc, *pyKeys, *pyIter, *pyItem;
    Py_ssize_t keys_len;

    const char *moduleName = PyModule_GetName(pyModule);

    // check for zbx_module_item_list function in module
    if(NULL == (pyFunc = PyObject_GetAttrString(pyModule, "zbx_module_item_list")) || 0 == PyFunction_Check(pyFunc)) {
        zabbix_log(LOG_LEVEL_INFORMATION, "function not found: %s.zbx_module_item_list", moduleName);
    } else {
        // call function
        if(NULL == (pyKeys = PyObject_CallObject(pyFunc, NULL)) || !PyList_Check(pyKeys)) {
            perrorf(NULL, "error calling %s.zbx_module_item_list", moduleName);
        } else {
            if(NULL == (pyIter = PyObject_GetIter(pyKeys))) {
                perrorf(NULL, "error iterating key list returned by %s.zbx_module_item_list", moduleName);
            } else {
                // alloc item list
                keys_len = PyObject_Length(pyKeys);
                keys = calloc(keys_len+1, sizeof(ZBX_METRIC));
                item = keys;

                // marshall items
                while (pyItem = PyIter_Next(pyIter)) {
                    PyObject *pyValue;

                    // TODO: validate AgentItem

                    // marshall key
                    pyValue = PyObject_GetAttrString(pyItem, "key");
                    item->key = PyUnicode_AsUTF8(pyValue);
                    Py_DECREF(pyValue);

                    // marshall flags
                    pyValue = PyObject_GetAttrString(pyItem, "key");
                    item->flags = (int) PyLong_AsLong(pyValue);
                    Py_DECREF(pyValue);

                    // always callback to the marshaller function
                    item->function = PYTHON_MARSHAL;

                    // marshall test parameter
                    pyValue = PyObject_GetAttrString(pyItem, "test_param");
                    item->test_param = PyUnicode_AsUTF8(pyValue);
                    Py_DECREF(pyValue);
                    
                    Py_DECREF(pyItem);
                    item++;
                }

                Py_DECREF(pyIter);
            }
        }

        if (pyKeys)
            Py_DECREF(pyKeys);
    }

    return keys;
}
