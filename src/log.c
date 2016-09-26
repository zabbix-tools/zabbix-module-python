#include "libzbxpython.h"

static void _vlogf(int level, const char *format, va_list args)
{
    char    msg[MAX_STRING_LEN];

    // parse message string
    zbx_vsnprintf((char*)&msg, sizeof(msg), format, args);

    // log message
    zabbix_log(level, "Python: %s", msg);
}

static void _logf(int level, const char *format, ...)
{
    va_list args;

    va_start (args, format);
    _vlogf(level, format, args);
    va_end(args);
}

void vinfof(const char *format, va_list args)
{
    _vlogf(LOG_LEVEL_INFORMATION, format, args);
}

void infof(const char *format, ...)
{
    va_list args;

    va_start (args, format);
    _vlogf(LOG_LEVEL_INFORMATION,format, args);
    va_end(args);
}

void vdebugf(const char *format, va_list args)
{
    _vlogf(LOG_LEVEL_DEBUG, format, args);
}

void debugf(const char *format, ...)
{
    va_list args;

    va_start (args, format);
    _vlogf(LOG_LEVEL_DEBUG, format, args);
    va_end(args);
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

    // print error message and set result message
    va_start(args, format);
    verrorf(result, format, args);
    va_end(args);

    // log python error message
    if (PyErr_Occurred()) {
        PyObject *pyType, *pyValue, *pyTraceback, *pyString;
        
        PyErr_Fetch(&pyType, &pyValue, &pyTraceback);

        // log error description
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
