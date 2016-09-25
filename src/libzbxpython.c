#include "libzbxpython.h"


// Define custom keys
static ZBX_METRIC keys[] = {
    {"python.modver",   0,  PYTHON_MODVER,      NULL},
    {"python.version",  0,  PYTHON_VERSION,     NULL},
    {NULL}
};

PyObject    *pySysModule = NULL;
PyObject    *pyAgentModule = NULL;

static char *python_version_string = NULL;

// Required Zabbix module functions
int         zbx_module_api_version()                { return ZBX_MODULE_API_VERSION; }
void        zbx_module_item_timeout(int timeout)    { return; }
ZBX_METRIC  *zbx_module_item_list()                 { return keys; }

int  zbx_module_init() {
    PyObject *pyValue = NULL;

    Py_Initialize();

    // load sys module
    if(NULL == (pySysModule = python_import_module(NULL, "sys")))
        return ZBX_MODULE_FAIL;

    // update module search paths
    python_add_module_path(NULL, ".");

    // load zabbix module module
    if(NULL == (pyAgentModule = python_import_module(NULL, PYTHON_MODULE)))
        return ZBX_MODULE_FAIL;

    // get python runtime version
    pyValue = PyObject_GetAttrString(pyAgentModule, "python_version_string");
    if(NULL == pyValue) {
        errorf(NULL, "attribute not found: " PYTHON_MODULE ".python_version_string");
        return ZBX_MODULE_FAIL;
    }

    python_version_string = PyUnicode_AsUTF8(pyValue);
    Py_DECREF(pyValue);

    // load agent modules
    if (NULL == get_module_item_list(NULL, pyAgentModule))
        return ZBX_MODULE_FAIL;

    return ZBX_MODULE_OK; 
}

int zbx_module_uninit()
{
    if (pySysModule)
        Py_DECREF(pySysModule);

    Py_Finalize();

    return ZBX_MODULE_OK;
}

int verrorf(AGENT_RESULT *result, const char *format, va_list args)
{
    char    msg[MAX_STRING_LEN];

    // parse message string
    zbx_vsnprintf((char*)&msg, sizeof(msg), format, args);

    // log message
    zabbix_log(LOG_LEVEL_ERR, "Python: %s", msg);

    if (NULL != result)
        SET_MSG_RESULT(result, strdup(msg));

    return SYSINFO_RET_FAIL;
}

int errorf(AGENT_RESULT *result, const char *format, ...)
{
    va_list args;

    va_start (args, format);
    verrorf(result, format, args);
    va_end(args);

    return SYSINFO_RET_FAIL;
}

int perrorf(AGENT_RESULT *result, const char *format, ...)
{
    va_list args;

    // print error message
    va_start(args, format);
    verrorf(result, format, args);
    va_end(args);

    // print python error message
    if (PyErr_Occurred()) {
        PyObject *pyType, *pyValue, *pyTraceback, *pyString;
        
        PyErr_Fetch(&pyType, &pyValue, &pyTraceback);
        if (NULL != (pyString = PyObject_Str(pyValue ? pyValue : pyType))) {
            errorf(NULL, PyUnicode_AsUTF8(pyString));
            Py_DECREF(pyString);
        }

        if (pyType)
            Py_DECREF(pyType);
        
        if (pyValue)
            Py_DECREF(pyValue);
        
        if (pyTraceback)
            Py_DECREF(pyTraceback);
    }

    return SYSINFO_RET_FAIL;
}

/*
 * Custom key: python.modver
 *
 * Returns the version string of the libzbxpython module.
 *
 * Parameters:
 *
 * Returns: s
 */
int PYTHON_MODVER(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    SET_STR_RESULT(result, strdup(PACKAGE_STRING ", compiled with Zabbix " ZABBIX_VERSION " and Python " PY_VERSION));
    return SYSINFO_RET_OK;
}

/*
 * Custom key: python.version
 *
 * Returns the version string of the loaded python runtime.
 *
 * Parameters:
 *
 * Returns: s
 */
int PYTHON_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    SET_STR_RESULT(result, strdup(python_version_string));
    return SYSINFO_RET_OK;
}
