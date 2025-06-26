#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <Python.h>
#include <numpy/arrayobject.h>

#include "a_bc.hpp"
#include <hpx/hpx_init.hpp>      // declares hpx::start / hpx::stop
#include <atomic>
#include <cstring>
#include <vector>

/* ------------------------------------------------------------------ */
/*  HPX runtime control                                               */
/* ------------------------------------------------------------------ */
static std::atomic<bool> hpx_running{false};

static PyObject* py_hpx_init(PyObject*, PyObject*)
{
    if (!hpx_running) {
        import_array();                                    // NumPy C-API
        char prog[] = "symhpx";
        char* argv[] = {prog, nullptr};
        int argc = 1;
        hpx::local::start(nullptr, argc, argv, hpx::init_params{});                          // non-blocking
        hpx_running = true;
    }
    Py_RETURN_NONE;
}

static PyObject* py_hpx_finalize(PyObject*, PyObject*)
{
    if (hpx_running) {
        hpx::local::stop();                                        // shut down
        hpx_running = false;
    }
    Py_RETURN_NONE;
}

/* ------------------------------------------------------------------ */
/*  Kernel wrapper                                                    */
/* ------------------------------------------------------------------ */
static PyObject* py_cpp__a_bc(PyObject*, PyObject* args)
{
    PyArrayObject *o_vr, *o_va, *o_vb, *o_vc;
    double d;

    if (!PyArg_ParseTuple(args, "O!O!O!O!d",
                          &PyArray_Type, &o_vr,
                          &PyArray_Type, &o_va,
                          &PyArray_Type, &o_vb,
                          &PyArray_Type, &o_vc,
                          &d))
        return nullptr;

    const int n = static_cast<int>(PyArray_DIMS(o_va)[0]);

    /* zero-copy views */
    double* vr = static_cast<double*>(PyArray_DATA(o_vr));
    double* va = static_cast<double*>(PyArray_DATA(o_va));
    double* vb = static_cast<double*>(PyArray_DATA(o_vb));
    double* vc = static_cast<double*>(PyArray_DATA(o_vc));

    std::vector<double> vv_r(vr, vr + n);
    std::vector<double> vv_a(va, va + n);
    std::vector<double> vv_b(vb, vb + n);
    std::vector<double> vv_c(vc, vc + n);

    cpp__a_bc(vv_r, vv_a, vv_b, vv_c, d);

    std::memcpy(vr, vv_r.data(), sizeof(double) * n);
    Py_RETURN_NONE;
}

/* ------------------------------------------------------------------ */
/*  Module boiler-plate                                               */
/* ------------------------------------------------------------------ */
static PyMethodDef methods[] = {
    {"hpx_init",     py_hpx_init,     METH_NOARGS,  "Start HPX runtime"},
    {"hpx_finalize", py_hpx_finalize, METH_NOARGS,  "Stop  HPX runtime"},
    {"cpp__a_bc",    py_cpp__a_bc,    METH_VARARGS, "Vector kernel"},
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef moddef = {
    PyModuleDef_HEAD_INIT, "symhpx", nullptr, -1, methods
};

PyMODINIT_FUNC PyInit_symhpx(void) { return PyModule_Create(&moddef); }
