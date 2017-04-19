#include "libzbxpython.h"

/* Define custom keys */
static ZBX_METRIC  *keys = NULL;

/* Python complimentary module (zabbix_module) */
PyObject *pyAgentModule = NULL;

/* Python router function (zabbix_module.route) */
PyObject *pyRouterFunc = NULL;

/* pid that called zbx_module_init */
pid_t init_pid = 0;

/******************************************************************************
 *                                                                            *
 * Function: PYTHON_MODVER                                                    *
 *                                                                            *
 * Purpose: Item key python.modver                                            *
 *          Returns the version string of this module.                        *
 *                                                                            *
 * Parameters: None                                                           *
 *                                                                            *
 * Return value: s                                                            *
 *                                                                            *
 ******************************************************************************/
static int 
PYTHON_MODVER(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    SET_STR_RESULT(result, strdup(PACKAGE_STRING ", compiled with Zabbix " ZABBIX_VERSION " and Python " PY_VERSION));
    return SYSINFO_RET_OK;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_metric_len                                                   *
 *                                                                            *
 * Purpose: Returns the number of items in a ZBX_METRIC array, not including  *
 *          the NULL terminator.                                              *
 *                                                                            *
 * Parameters: m - list of items supported by a module                        *
 *                                                                            *
 * Return value: (int)                                                        *
 *                                                                            *
 ******************************************************************************/
static int
zbx_metric_len(ZBX_METRIC *m)
{
    int count = 0;
    ZBX_METRIC *p = NULL;

    for (p = m; p && p->key; p++)
        count++;

    return count;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_metric_concat                                                *
 *                                                                            *
 * Purpose: Concatenates two ZBX_METRIC arrays into a newly allocated         *
 *          array with shallow copies of the original metrics.                *
 *                                                                            *
 *          It is the caller's repsonsibility to free the returned            *
 *          reference.                                                        *
 *                                                                            *
 * Return value: (ZBX_METRIC*)                                                *
 *                                                                            *
 ******************************************************************************/
static ZBX_METRIC
*zbx_metric_concat(ZBX_METRIC *a, ZBX_METRIC *b) 
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

    return m;
}

// Required Zabbix module functions
#ifdef ZBX_MODULE_API_VERSION
// Recommended Zabbix 3.2.x
int         zbx_module_api_version()                { return ZBX_MODULE_API_VERSION; }
#else
// Zabbix 3.0 support
int         zbx_module_api_version()                { return ZBX_MODULE_API_VERSION_ONE; }
#endif
ZBX_METRIC  *zbx_module_item_list()                 { return keys; }

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_init                                                  *
 *                                                                            *
 * Purpose: Called by Zabbix when the module is first loaded. This            *
 *          function will initialize the Python runtime and initialize        *
 *          all installed Python modules.                                     *
 *                                                                            *
 * Return value: (int) ZBX_MODULE_OK or ZBX_MODULE_FAIL                       *
 *                                                                            *
 ******************************************************************************/
int
zbx_module_init() {
    ZBX_METRIC  *m = NULL, *n = NULL;
    PyObject    *pyValue = NULL;

    // cache initialization process pid for managing forks later
    if (init_pid)
        zabbix_log(LOG_LEVEL_WARNING, "module already initialized by [%i]", init_pid);

    init_pid = getpid();

    // initialize python runtime
    PyImport_AppendInittab("zabbix_runtime", PyInit_zabbix_runtime);
    Py_Initialize();

    // import complimentary zabbix_module Python lib
    if(NULL == (pyAgentModule = python_import_module(PYTHON_MODULE)))
        return ZBX_MODULE_FAIL;

    // advise python module of agent module load path
    pyValue = PyUnicode_FromString(ZABBIX_PYTHON_MODULE_PATH);
    if(-1 == PyObject_SetAttrString(pyAgentModule, "zabbix_module_path", pyValue)) {
        zabbix_log(LOG_LEVEL_ERR, "failed to set " PYTHON_MODULE ".zabbix_module_path");
        python_log_error(NULL);
        return ZBX_MODULE_FAIL;
    }
    Py_DECREF(pyValue);

    // call zbx_module_init in python module
    if (ZBX_MODULE_FAIL == python_module_init(pyAgentModule)) {
        zabbix_log(LOG_LEVEL_ERR, "failed to initialize " PYTHON_MODULE " python module");
        return ZBX_MODULE_FAIL;
    }

    // cache router function
    if (NULL == (pyRouterFunc = PyObject_GetAttrString(pyAgentModule, "route"))) {
        zabbix_log(LOG_LEVEL_ERR, "function not found: " PYTHON_MODULE ".route");
        return ZBX_MODULE_FAIL;
    }

    // check item_timeout attribute
    if (NULL == PyObject_GetAttrString(pyAgentModule, "item_timeout")) {
        zabbix_log(LOG_LEVEL_ERR, "attribute not found: " PYTHON_MODULE ".item_timeout");
        return ZBX_MODULE_FAIL;
    }

    // init builtin items
    m = calloc(2, sizeof(ZBX_METRIC));
    m->key = strdup("python.modver");
    m->function = PYTHON_MODVER;

    // init items from zabbix_module python module
    if(NULL == (n = python_module_item_list(pyAgentModule))) {
        zabbix_log(LOG_LEVEL_ERR, "cannot read item list from " PYTHON_MODULE " python module");    
        return ZBX_MODULE_FAIL;
    }
    
    // merge builtin and in modular items
    keys = zbx_metric_concat(m, n);
    free(m);
    free(n);

    return ZBX_MODULE_OK; 
}

int zbx_module_uninit()
{
    zabbix_log(LOG_LEVEL_DEBUG, "finalizing Python runtime");
    Py_Finalize();

    return ZBX_MODULE_OK;
}

void zbx_module_item_timeout(int timeout)
{
    PyObject *pyValue = NULL;

    if(NULL == (pyValue = PyLong_FromLong((long int) timeout))) {
        python_log_error(NULL);
    } else {
        if (-1 == PyObject_SetAttrString(pyAgentModule, "item_timeout", pyValue)) {
            zabbix_log(LOG_LEVEL_ERR, "failed to set item timeout");
            python_log_error(NULL);
        }

        Py_DECREF(pyValue);
    }
}
