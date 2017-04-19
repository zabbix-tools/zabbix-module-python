#include "libzbxpython.h"

/* flag if python has initialized on this pid */
static int ran_after_fork = 0;

/******************************************************************************
 *                                                                            *
 * Function: python_marshall_request                                          *
 *                                                                            *
 * Purpose: marshall a Zabbix AGENT_REQUEST C struct to a Python              *
 *          zabbix_module.AgentRequest.                                       *
 *                                                                            *
 * Return value: (PyObject*) zabbix_module.Agentrequest                       *
 *                                                                            *
 ******************************************************************************/
static PyObject*
python_marshall_request(AGENT_REQUEST *request)
{
    PyObject *pyKey = NULL, *pyParams, *pyArgs, *pyRequestClass, *pyRequest;
    int nparam;

    // clear errors
    PyErr_Clear();

    // marshal request key
    pyKey = PyUnicode_FromString(request->key);

    // marshal each request param
    nparam   = get_rparams_num(request);
    pyParams = PyList_New(nparam);
    
    for(int i = 0; i < nparam; i++)
        PyList_SetItem(pyParams, i, PyUnicode_FromString(get_rparam(request, i)));

    // instanciate AgentRequest objecy
    pyArgs         = PyTuple_Pack(2, pyKey, pyParams);
    pyRequestClass = PyObject_GetAttrString(pyAgentModule, "AgentRequest");
    pyRequest      = PyObject_CallObject(pyRequestClass, pyArgs);

    // log any errors
    python_log_error(NULL);

    // clean up
    if (pyKey)
        Py_DECREF(pyKey);

    if (pyRequestClass)
        Py_DECREF(pyRequestClass);

    if (pyArgs)
        Py_DECREF(pyArgs);

    if (pyParams)
        Py_DECREF(pyParams);
    
    return pyRequest;
}

/******************************************************************************
 *                                                                            *
 * Function: PYTHON_ROUTER                                                    *
 *                                                                            *
 * Purpose: marshall a Zabbix agent request to its registered Python handler  *
 *          via call to zabbix_module.route                                   *
 *                                                                            *
 * Return value: SYSINFO_RET_FAIL | SYSINFO_RET_OK                            *
 *                                                                            *
 ******************************************************************************/
int
PYTHON_ROUTER(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;
    PyObject    *pyRequest = NULL, *pyArgs = NULL, *pyValue = NULL;

    /*
     * After zbx_module_init is called from the Zabbix agent it forks each of
     * its worker processes. Python requires that PyOS_AfterFork is called after
     * a fork to update some internal state. The Zabbix ABI does not offer any
     * opportunity to trigger code after it forks so we call PyOS_AfterFork in
     * the first item check that occurs in each thread, before engaging the
     * Python runtime.
     */
    if (0 == ran_after_fork) {
        if (getpid() != init_pid) {
            zabbix_log(LOG_LEVEL_DEBUG, "calling PyOS_AfterFork");
            PyOS_AfterFork();
        }

        ran_after_fork = 1;
    }

    // marshal agent request to python object
    if(NULL == (pyRequest = python_marshall_request(request))) {
        SET_MSG_RESULT(result, "cannot marshall agent request to python object");
        goto out;
    }
    
    // create func args for python router function
    if (NULL == (pyArgs = PyTuple_Pack(1, pyRequest))) {
        if(0 == python_log_error(result))
            SET_MSG_RESULT(result, "cannot pack function arguments");

        goto out;
    }

    // call router function
    zabbix_log(LOG_LEVEL_DEBUG, "routing request to python module");
    if (NULL == (pyValue = PyObject_CallObject(pyRouterFunc, pyArgs))) {
        if (0 == python_log_error(result))
            SET_MSG_RESULT(result, "unknown error calling python router function");

        goto out;
    }

    // unmarshall Python value to a Zabbix return value
    if(PyObject_TypeCheck(pyValue, &PyLong_Type)) {
        zabbix_log(LOG_LEVEL_DEBUG, "unmarshalling result value as unsigned 64bit integer");
        SET_UI64_RESULT(result, PyLong_AsLongLong(pyValue));
    } else if (PyObject_TypeCheck(pyValue, &PyFloat_Type)) {
        zabbix_log(LOG_LEVEL_DEBUG, "unmarshalling result value as double");
        SET_DBL_RESULT(result, PyFloat_AsDouble(pyValue));
    } else {
        zabbix_log(LOG_LEVEL_DEBUG, "unmarshalling result value as string");
        SET_STR_RESULT(result, python_str(pyValue)); /* zabbix will free */
    }

    ret = SYSINFO_RET_OK;

out:
    if (pyRequest)
        Py_DECREF(pyRequest);

    if (pyArgs)
        Py_DECREF(pyArgs);

    if (pyValue)
        Py_DECREF(pyValue);

    return ret;
}
