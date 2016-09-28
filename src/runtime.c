#include "libzbxpython.h"

// log to the zabbix runtime log file
static PyObject *zabbix_runtime_log(PyObject *self, PyObject *args)
{
	const int level;
	const char *str;

	if(!PyArg_ParseTuple(args, "is", &level, &str)) {
		python_log_error(NULL);
	} else {
		zabbix_log(level, str);
	}

	Py_RETURN_NONE;
}

// define module methods
static PyMethodDef zabbix_runtime_methods[] = {
    {"log",  zabbix_runtime_log, METH_VARARGS, "Log a message to the Zabbix log file."},
    {NULL, NULL, 0, NULL}
};

// define runtime module
static struct PyModuleDef zabbix_runtime_module = { 
	PyModuleDef_HEAD_INIT,
	"zabbix_runtime",
	NULL,
	-1,
	zabbix_runtime_methods
};

// init runtime module
PyMODINIT_FUNC PyInit_zabbix_runtime(void)
{
    return PyModule_Create(&zabbix_runtime_module);
}
