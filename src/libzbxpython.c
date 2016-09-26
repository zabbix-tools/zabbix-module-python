#include "libzbxpython.h"

// Define custom keys
static ZBX_METRIC  *keys = NULL;

PyObject    *pySysModule = NULL;
PyObject    *pyAgentModule = NULL;
PyObject    *pyRouterFunc = NULL;

static char *python_version_string = NULL;

pid_t init_pid = 0;

// Required Zabbix module functions
int         zbx_module_api_version()                { return ZBX_MODULE_API_VERSION; }
void        zbx_module_item_timeout(int timeout)    { return; }
ZBX_METRIC  *zbx_module_item_list()                 { return keys; }

int  zbx_module_init() {
    ZBX_METRIC  *m = NULL;
    PyObject    *pyValue = NULL;

    // cache initialization process pid for managing forks later
    init_pid = getpid();

    // initialize python runtime
    Py_Initialize();

    // import python sys module
    if(NULL == (pySysModule = python_import_module(NULL, "sys")))
        return ZBX_MODULE_FAIL;

    // update python module search paths
    python_add_module_path(NULL, ZABBIX_MODULE_PATH);

    // import python module for zabbix
    if(NULL == (pyAgentModule = python_import_module(NULL, PYTHON_MODULE)))
        return ZBX_MODULE_FAIL;

    // advise module of module path
    pyValue = PyUnicode_FromString(ZABBIX_MODULE_PATH);
    PyObject_SetAttrString(pyAgentModule, "zabbix_module_path", pyValue);
    Py_DECREF(pyValue);

    // cache router function
    if (NULL == (pyRouterFunc = PyObject_GetAttrString(pyAgentModule, "route"))) {
        errorf(NULL, "function not found: " PYTHON_MODULE ".route");
        return ZBX_MODULE_FAIL;
    }

    // init builtin items
    keys = calloc(2, sizeof(ZBX_METRIC));
    keys[0].key = strdup("python.modver");
    keys[0].function = PYTHON_MODVER;

    // init items from zabbix_module python module
    if(NULL == (m = get_module_item_list(pyAgentModule))) {
        errorf(NULL, "cannot read item list from " PYTHON_MODULE " python module");    
        return ZBX_MODULE_FAIL;
    }
     
    keys = zbx_metric_merge(keys, m);

    return ZBX_MODULE_OK; 
}

int zbx_module_uninit()
{
    if (pySysModule)
        Py_DECREF(pySysModule);

    Py_Finalize();

    return ZBX_MODULE_OK;
}

int zbx_metric_len(ZBX_METRIC *m)
{
    int count = 0;
    ZBX_METRIC *p = NULL;

    for (p = m; p && p->key; p++)
        count++;

    return count;
}

ZBX_METRIC *zbx_metric_merge(ZBX_METRIC *a, ZBX_METRIC *b) 
{
    int len_a;
    int len_b;

    ZBX_METRIC *m, *p_m, *p_a, *p_b;

    len_a = zbx_metric_len(a);
    len_b = zbx_metric_len(b);

    m = (ZBX_METRIC*) calloc(len_a + len_b + 1, sizeof(ZBX_METRIC));
    p_m = m;

    for(p_a = a; p_a && p_a->key; p_a++)
        (*p_m++) = *p_a;

    for(p_b = b; p_b && p_b->key; p_b++)
        (*p_m++) = *p_b;

    free(a);
    free(b);

    return m;
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
