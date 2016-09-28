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

int python_module_init(PyObject *pyModule)
{
    int ret = ZBX_MODULE_FAIL;
    PyObject *pyFunc;

    const char *moduleName = PyModule_GetName(pyModule);    

    // check for zbx_module_item_list function in module
    if(NULL == (pyFunc = PyObject_GetAttrString(pyModule, "zbx_module_init")) || 0 == PyFunction_Check(pyFunc)) {
        zabbix_log(LOG_LEVEL_INFORMATION, "function not found: %s.zbx_module_init", moduleName);
    } else {
        // call function
        if(NULL == (PyObject_CallObject(pyFunc, NULL))) {
            perrorf(NULL, "error calling %s.zbx_module_init", moduleName);
        } else {
            ret = ZBX_MODULE_OK;
        }

        Py_DECREF(pyFunc);
    }

    return ret;
}

ZBX_METRIC *python_module_item_list(PyObject *pyModule)
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
