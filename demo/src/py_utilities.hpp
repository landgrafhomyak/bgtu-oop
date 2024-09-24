#pragma once

#include <cstdarg>
#include <Python.h>

namespace LdH::Studying::BGTU::OOP::Demo {
    PyObject *PyObject_VaCallMethodArgsKwargsVararg(PyObject *self, char const* method_name, char const *args_fmt, char const *kwargs_fmt, va_list varargs);
    PyObject *PyObject_CallMethodArgsKwargsVararg(PyObject *self, char const* method_name, char const *args_fmt, char const *kwargs_fmt, ...);
    bool PyObject_CallMethodArgsKwargsVararg_Void(PyObject *self, char const* method_name, char const *args_fmt, char const *kwargs_fmt, ...);
};