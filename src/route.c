#include "libzbxpython.h"

static int ran_after_fork = 0;

int PYTHON_MARSHAL(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int ret = SYSINFO_RET_FAIL;

    PyObject *pyArgs, *pyRequestClass, *pyRequest, *pyValue, *pyKey, *pyParams;
    int nparam, i;

    /*
     * After zbx_module_init is called from the Zabbix agent it forks each of
     * its worker processes. Python requires that PyOS_AfterFork is called after
     * a fork to update some internal state. The Zabbix ABI does not offer any
     * opportunity to trigger code after it forks so we call PyOS_AfterFork in
     * the first item check that occurs in each thread, before engaging the
     * Python runtime.
     */
    if (0 == ran_after_fork) {
        if (getpid() != init_pid)
            PyOS_AfterFork();

        ran_after_fork = 1;
    }

    // convert key to python string
    if(NULL == (pyKey = PyUnicode_FromString(request->key))) {
        perrorf(result, "unable to marshal request key to python");
        goto out;
    }

    // convert params to python list
    nparam = get_rparams_num(request);
    if(NULL == (pyParams = PyList_New(nparam))) {
        perrorf(result, "unable to create new python list");
        goto out;
    }

    for(int i = 0; i < nparam; i++) {
        if(NULL == (pyValue = PyUnicode_FromString(get_rparam(request, i)))) {
            perrorf(result, "unable to marshal request parameter to python");
            goto out;
        }

        if(-1 == PyList_SetItem(pyParams, i, pyValue)) {
            perrorf(result, "unable to set python parameter value");
            goto out;
        }
    }

    // create init args for AgentRequest
    if(NULL == (pyArgs = PyTuple_New(2))) {
        perrorf(result, "unable to create args to marshal Agentrequest");
        goto out;
    }

    if(-1 == (PyTuple_SetItem(pyArgs, 0, pyKey)) 
        || -1 == (PyTuple_SetItem(pyArgs, 1, pyParams))) {
        perrorf(result, "unable to set python arguments to marshal AgentRequest");
        goto out;
    }

    // get "AgentRequest" class
    if(NULL == (pyRequestClass = PyObject_GetAttrString(pyAgentModule, "AgentRequest"))) {
        perrorf(result, "unable to fetch " PYTHON_MODULE ".AgentRequest class");
        goto out;
    }

    // instanciate AgentRequest
    if(NULL == (pyRequest = PyObject_CallObject(pyRequestClass, pyArgs))) {
        perrorf(result, "unable to create new " PYTHON_MODULE ".AgentRequest");
        goto out;
    }
    
    // create func args from AgentRequest
    if(NULL == (pyArgs = PyTuple_New(1))) {
        perrorf(result, "unable to create args for router function");
        goto out;
    }

    if(-1 == PyTuple_SetItem(pyArgs, 0, pyRequest)) {
        perrorf(result, "unable to set python arguments for router function");
        goto out;
    }

    // call router function
    // TODO: only send py error to zabbix
    if (NULL == (pyValue = PyObject_CallObject(pyRouterFunc, pyArgs))) {
        perrorf(result, "error calling python function for key: %s", request->key);
        goto out;
    }

    SET_STR_RESULT(result, python_str(pyValue));
    ret = SYSINFO_RET_OK;

out:
    if (pyArgs)         Py_DECREF(pyArgs);
    if (pyRequestClass) Py_DECREF(pyRequestClass);
    if (pyRequest)      Py_DECREF(pyRequest);
    if (pyKey)          Py_DECREF(pyKey);
    if (pyValue)        Py_DECREF(pyValue);
    if (pyParams)       Py_DECREF(pyParams);

    return SYSINFO_RET_OK;
}
