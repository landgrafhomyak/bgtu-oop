#include <cstdarg>
#include <cstring>
#include <Python.h>
#include "py_utilities.hpp"

namespace LdH::Studying::BGTU::OOP::Demo {
    PyObject *PyObject_VaCallMethodArgsKwargsVararg(PyObject *self, char const *method_name, char const *args_fmt, char const *kwargs_fmt, va_list varargs) {
        size_t args_fmt_size = std::strlen(args_fmt), kwargs_fmt_size = std::strlen(kwargs_fmt);
        char *all_fmt = (char *) PyMem_Malloc(sizeof(char) * (args_fmt_size + kwargs_fmt_size + 6 + 1));
        if (all_fmt == nullptr) {
            PyErr_NoMemory();
            return nullptr;
        }

        all_fmt[0] = '(';
        all_fmt[1] = '(';
        std::strcpy(all_fmt + 2, args_fmt);
        all_fmt[2 + args_fmt_size + 0] = ')';
        all_fmt[2 + args_fmt_size + 1] = '{';
        std::strcpy(all_fmt + 2 + args_fmt_size + 2, kwargs_fmt);
        all_fmt[2 + args_fmt_size + 2 + kwargs_fmt_size + 0] = '}';
        all_fmt[2 + args_fmt_size + 2 + kwargs_fmt_size + 1] = ')';
        all_fmt[2 + args_fmt_size + 2 + kwargs_fmt_size + 2] = '\0';

        PyObject *args_kwargs = Py_VaBuildValue(all_fmt, varargs);
        PyMem_Free(all_fmt);
        if (args_kwargs == nullptr)
            return nullptr;

        PyObject *args_o = PyTuple_GET_ITEM(args_kwargs, 0), *kwargs_o = PyTuple_GET_ITEM(args_kwargs, 1);
        Py_INCREF(args_o);
        Py_INCREF(kwargs_o);
        Py_DECREF(args_kwargs);

        PyObject *method_o = PyObject_GetAttrString(self, method_name);
        if (method_o == nullptr) {
            Py_DECREF(args_o);
            Py_DECREF(kwargs_o);
            return nullptr;
        }

        PyObject *ret = PyObject_Call(method_o, args_o, kwargs_o);
        Py_INCREF(args_o);
        Py_INCREF(kwargs_o);
        Py_INCREF(method_o);
        if (ret == nullptr)
            return nullptr;

        return ret;
    }

    PyObject *PyObject_CallMethodArgsKwargsVararg(PyObject *self, char const *method_name, char const *args_fmt, char const *kwargs_fmt, ...) {
        va_list varargs;
        va_start(varargs, kwargs_fmt);
        PyObject *ret = PyObject_VaCallMethodArgsKwargsVararg(self, method_name, args_fmt, kwargs_fmt, varargs);
        va_end(varargs);
        return ret;
    }

    bool PyObject_CallMethodArgsKwargsVararg_Void(PyObject *self, char const *method_name, char const *args_fmt, char const *kwargs_fmt, ...) {
        va_list varargs;
        va_start(varargs, kwargs_fmt);
        PyObject *ret = PyObject_VaCallMethodArgsKwargsVararg(self, method_name, args_fmt, kwargs_fmt, varargs);
        va_end(varargs);
        if (ret == nullptr) {
            return true;
        } else {
            Py_DECREF(ret);
            return false;
        }
    }
};