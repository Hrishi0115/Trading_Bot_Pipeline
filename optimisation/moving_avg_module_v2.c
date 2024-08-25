#include <Python.h>
#include <stdlib.h>  // For malloc and free

// Function to update the moving average windows and calculate the new averages using a circular buffer
void update_moving_averages(double *short_mavg_window, double *long_mavg_window, int short_len, int long_len, double new_price, double *short_mavg, double *long_mavg, int *short_index, int *long_index, double *short_sum, double *long_sum) {
    // Update short moving average window
    *short_sum -= short_mavg_window[*short_index];
    short_mavg_window[*short_index] = new_price;
    *short_sum += new_price;
    *short_index = (*short_index + 1) % short_len;
    *short_mavg = *short_sum / short_len;

    // Update long moving average window
    *long_sum -= long_mavg_window[*long_index];
    long_mavg_window[*long_index] = new_price;
    *long_sum += new_price;
    *long_index = (*long_index + 1) % long_len;
    *long_mavg = *long_sum / long_len;
}

// Wrapper for the update_moving_averages function
static PyObject* py_update_moving_averages(PyObject* self, PyObject* args) {
    PyObject *short_mavg_list, *long_mavg_list;
    double new_price;
    int short_index, long_index;
    double short_sum, long_sum;

    if (!PyArg_ParseTuple(args, "OOdiidd", &short_mavg_list, &long_mavg_list, &new_price, &short_index, &long_index, &short_sum, &long_sum)) {
        return NULL;
    }

    int short_len = (int)PyList_Size(short_mavg_list);
    int long_len = (int)PyList_Size(long_mavg_list);

    double *short_mavg_window = (double *)malloc(short_len * sizeof(double));
    double *long_mavg_window = (double *)malloc(long_len * sizeof(double));

    if (short_mavg_window == NULL || long_mavg_window == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Memory allocation failed");
        return NULL;
    }

    for (int i = 0; i < short_len; i++) {
        short_mavg_window[i] = PyFloat_AsDouble(PyList_GetItem(short_mavg_list, i));
    }

    for (int i = 0; i < long_len; i++) {
        long_mavg_window[i] = PyFloat_AsDouble(PyList_GetItem(long_mavg_list, i));
    }

    double short_mavg, long_mavg;
    update_moving_averages(short_mavg_window, long_mavg_window, short_len, long_len, new_price, &short_mavg, &long_mavg, &short_index, &long_index, &short_sum, &long_sum);

    free(short_mavg_window);
    free(long_mavg_window);

    return Py_BuildValue("ddiiidd", short_mavg, long_mavg, short_index, long_index, short_sum, long_sum);
}

// Method definitions
static PyMethodDef MovingAvgMethods[] = {
    {"update_moving_averages", py_update_moving_averages, METH_VARARGS, "Update moving averages"},
    {NULL, NULL, 0, NULL}
};

// Module definition
static struct PyModuleDef movingavgmodule = {
    PyModuleDef_HEAD_INIT,
    "moving_avg",
    NULL,
    -1,
    MovingAvgMethods
};

// Module initialization
PyMODINIT_FUNC PyInit_moving_avg(void) {
    return PyModule_Create(&movingavgmodule);
}
