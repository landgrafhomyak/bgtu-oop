#include <Python.h>
#include <LdH/studying/bgtu/oop/application.hpp>
#include "canvas.hpp"

namespace LdH::Studying::BGTU::OOP::Demo {
    struct ExchangePyObject {
    public:
        PyObject_HEAD
        QPainterPathCanvasProvider canvas;
        Interface::ApplicationCallbacks *const app;
        PyObject *signal_ref;
        PyObject *path_constructor_ref;

    private:
        static void dealloc(ExchangePyObject *self);

        template<typename callable_t>
        static PyObject *_run_catching_none(callable_t callable);

        template<typename ret_t, PyTypeObject *py_type, ret_t (converter)(PyObject *)>
        static bool _try_cast_py_object(ret_t *dst, PyObject *src);

        template<typename native_arg_t, PyTypeObject *py_type, native_arg_t (converter)(PyObject *), void (Interface::ApplicationCallbacks::*cb)(native_arg_t)>
        static PyObject *cbw__builder(ExchangePyObject *self, PyObject *arg0);

        static PyObject *cbw_set_param(ExchangePyObject *self, PyObject *args);

        static PyObject *cbw_resize_canvas(ExchangePyObject *self, PyObject *args);

        static PyMethodDef methods[];
    public:
        static PyTypeObject TypeObject;
    };
}
