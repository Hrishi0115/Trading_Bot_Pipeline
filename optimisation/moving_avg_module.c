#include <Python.h>
#include <stdlib.h>  // For malloc and free

// Function to calculate the sum of an array
double sum_array(double *array, int length) {
    double sum = 0.0;
    for (int i = 0; i < length; i++) {
        sum += array[i];
    }
    return sum;
}

// Function to update the moving average windows and calculate the new averages
void update_moving_averages(double *short_mavg_window, double *long_mavg_window, int short_len, int long_len, double new_price, double *short_mavg, double *long_mavg) {
    double prev_short_sum = sum_array(short_mavg_window, short_len);
    double prev_long_sum = sum_array(long_mavg_window, long_len);

    for (int i = 1; i < short_len; i++) {
        short_mavg_window[i - 1] = short_mavg_window[i];
    }
    short_mavg_window[short_len - 1] = new_price;

    for (int i = 1; i < long_len; i++) {
        long_mavg_window[i - 1] = long_mavg_window[i];
    }
    long_mavg_window[long_len - 1] = new_price;

    double new_short_sum = prev_short_sum + new_price - short_mavg_window[0];
    double new_long_sum = prev_long_sum + new_price - long_mavg_window[0];

    *short_mavg = new_short_sum / short_len;
    *long_mavg = new_long_sum / long_len;
}

// Wrapper for the update_moving_averages function
static PyObject* py_update_moving_averages(PyObject* self, PyObject* args) {
    PyObject *short_mavg_list, *long_mavg_list;
    double new_price;
    if (!PyArg_ParseTuple(args, "OOd", &short_mavg_list, &long_mavg_list, &new_price)) {
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
    update_moving_averages(short_mavg_window, long_mavg_window, short_len, long_len, new_price, &short_mavg, &long_mavg);

    free(short_mavg_window);
    free(long_mavg_window);

    return Py_BuildValue("dd", short_mavg, long_mavg);
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

// python setup.py build_ext --inplace - after including neccessary code in setup.py
