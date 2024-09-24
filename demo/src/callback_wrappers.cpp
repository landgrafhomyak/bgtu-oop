#include <climits>
#include <cstdio>
#include <new>
#include <Python.h>
#include <LdH/studying/bgtu/oop/application.hpp>
#include "callback_wrappers.hpp"

namespace LdH::Studying::BGTU::OOP::Demo {
    void ExchangePyObject::dealloc(ExchangePyObject *self) {
        self->app->~ApplicationCallbacks();
        Py_DECREF(self->path_constructor_ref);
        Py_DECREF(self->signal_ref);
        Py_TYPE(self)->tp_free(self);
    }


    template<typename callable_t>
    PyObject *ExchangePyObject::_run_catching_none(callable_t callable) {
        try {
            callable();
        } catch (std::exception const &err) {
            PySys_WriteStderr("%s\n", err.what());
            return Py_None;
        }

        return Py_None;
    }

    template<typename ret_t, PyTypeObject *py_type, ret_t (converter)(PyObject *)>
    bool ExchangePyObject::_try_cast_py_object(ret_t *dst, PyObject *src) {
        if (!PyObject_IsInstance(src, reinterpret_cast<PyObject *>(py_type))) {
            PyErr_Format(PyExc_TypeError, "Expected argument of type %R, got of type %R: %R", py_type, Py_TYPE(src), src);
            return true;
        }
        *dst = converter(src);
        if (PyErr_Occurred())
            return true;
        return false;
    }

    static signed _py_long_as_signed(PyObject *src) {
        long raw = PyLong_AsLong(src);
#       if LONG_MIN < INT_MIN
        if (raw < INT_MIN)
            goto LBL_OVERFLOW;
#       endif
#       if LONG_MAX > INT_MAX
        if (INT_MAX < raw)
            goto LBL_OVERFLOW;
#       endif
        return (signed) raw;

        LBL_OVERFLOW:
        PyErr_SetString(PyExc_OverflowError, "Specified int value bigger then native integer type can carry");
    }

    template<typename native_arg_t, PyTypeObject *py_type, native_arg_t (converter)(PyObject *), void (Interface::ApplicationCallbacks::*cb)(native_arg_t)>
    PyObject *ExchangePyObject::cbw__builder(ExchangePyObject *self, PyObject *arg0) {
        native_arg_t native_arg;

        if (_try_cast_py_object<native_arg_t, py_type, converter>(&native_arg, arg0))
            return nullptr;

        return _run_catching_none([=]() { (self->app->*cb)(native_arg); });
    }


    PyObject *ExchangePyObject::cbw_set_param(ExchangePyObject *self, PyObject *args) {
        char const *name;
        double value;

        if (!PyArg_ParseTuple(args, "sd", &name, &value)) {
            return nullptr;
        }

        return _run_catching_none([=]() { self->app->setParam(name, value); });
    }

    PyObject *ExchangePyObject::cbw_resize_canvas(ExchangePyObject *self, PyObject *args) {
        unsigned w, h;

        if (!PyArg_ParseTuple(args, "II", &w, &h)) {
            return nullptr;
        }

        self->canvas.cw = w;
        self->canvas.ch = h;

        return Py_None;
    }

    PyMethodDef ExchangePyObject::methods[] = {
            {"scroll_x",        (PyCFunction) &ExchangePyObject::cbw__builder<signed, &PyLong_Type, _py_long_as_signed, &Interface::ApplicationCallbacks::scrollX>,                METH_O},
            {"scroll_y",        (PyCFunction) &ExchangePyObject::cbw__builder<signed, &PyLong_Type, _py_long_as_signed, &Interface::ApplicationCallbacks::scrollY>,                METH_O},
            {"zoom_x",          (PyCFunction) &ExchangePyObject::cbw__builder<signed, &PyLong_Type, _py_long_as_signed, &Interface::ApplicationCallbacks::zoomX>,                  METH_O},
            {"zoom_y",          (PyCFunction) &ExchangePyObject::cbw__builder<signed, &PyLong_Type, _py_long_as_signed, &Interface::ApplicationCallbacks::zoomY>,                  METH_O},
            {"remove_param",    (PyCFunction) &ExchangePyObject::cbw__builder<char const *, &PyUnicode_Type, PyUnicode_AsUTF8, &Interface::ApplicationCallbacks::removeParam>,     METH_O},
            {"set_expression",  (PyCFunction) &ExchangePyObject::cbw__builder<char const *, &PyUnicode_Type, PyUnicode_AsUTF8, &Interface::ApplicationCallbacks::inputExpression>, METH_O},
            {"add_param",       (PyCFunction) &ExchangePyObject::cbw_set_param,                                                                                                    METH_VARARGS},
            {"set_canvas_size", (PyCFunction) &ExchangePyObject::cbw_resize_canvas,                                                                                                METH_VARARGS},
            {nullptr}
    };

    PyTypeObject ExchangePyObject::TypeObject = {
            .ob_base = PyVarObject_HEAD_INIT(nullptr, 0)
            .tp_name = "__main__.ExchangePyObject",
            .tp_basicsize = sizeof(ExchangePyObject),
            .tp_dealloc = (destructor) &ExchangePyObject::dealloc,
            .tp_methods =  ExchangePyObject::methods
    };

}