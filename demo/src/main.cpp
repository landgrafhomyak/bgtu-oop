#include <cstdio>

#include <Python.h>

#include <LdH/studying/bgtu/oop/application.hpp>
#include <LdH/studying/bgtu/oop/default_application_logic.hpp>
#include <LdH/studying/bgtu/oop/fun_expression_parser.hpp>

#include "widgets_py.hpp"
#include "callback_wrappers.hpp"
#include "canvas.hpp"


namespace LdH::Studying::BGTU::OOP::Demo {
    static bool run_widgets(PyObject **entry_point) {
        PyObject *compiled, *dict, *_eval_ret;

        compiled = Py_CompileString(widgets_py_source, "widgets.py", Py_file_input);
        if (compiled == nullptr) {
            return true;
        }

        dict = PyDict_New();
        if (dict == nullptr) {
            Py_DECREF(compiled);
            return true;
        }

        _eval_ret = PyEval_EvalCode(compiled, dict, dict);
        Py_DECREF(compiled);
        if (_eval_ret == nullptr) {
            Py_DECREF(dict);
            return true;
        } else {
            Py_DECREF(_eval_ret);
        }


        *entry_point = PyDict_GetItemString(dict, "main");
        Py_DECREF(dict);
        if (*entry_point == nullptr)
            return true;

        Py_INCREF((*entry_point));

        return false;
    }

    static PyObject *exchange_py_object_factory(PyObject *Py_UNUSED(module), PyObject *args, PyObject *kwargs) {
        static char const *kw_names[] = {
                "set_line_signal",
                "q_path_constructor",
                nullptr
        };

        PyObject *path_constructor;
        PyObject *set_line_signal;

        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", (char **) kw_names, &set_line_signal, &path_constructor))
            return nullptr;

        auto exchange_o = (ExchangePyObject *) ExchangePyObject::TypeObject.tp_alloc(&ExchangePyObject::TypeObject, 0);
        if (exchange_o == nullptr) {
            PyErr_NoMemory();
            return nullptr;
        }

        Py_INCREF(path_constructor);
        Py_INCREF(set_line_signal);

        exchange_o->signal_ref = set_line_signal;
        exchange_o->path_constructor_ref = path_constructor;

        new(&exchange_o->canvas) QPainterPathCanvasProvider{path_constructor, set_line_signal};

        const_cast<Interface::ApplicationCallbacks *&>(exchange_o->app) = default_application_logic->create(
                &exchange_o->canvas,
                fun_expression_parser,
                100,
                Interface::ApplicationCallbacks::ApplicationFactory::DEFAULT_VIEW_AREA_SETTINGS,
                Interface::ApplicationCallbacks::ApplicationFactory::DEFAULT_SCROLL_AND_ZOOM_SETTINGS
        );
        if (exchange_o->app == nullptr) {
            Py_DECREF(set_line_signal);
            Py_DECREF(path_constructor);
            ExchangePyObject::TypeObject.tp_free(exchange_o);
            PyErr_NoMemory();
            return nullptr;
        }

        return reinterpret_cast<PyObject *>(exchange_o);
    }

    static PyMethodDef exchange_py_object_factory_def = {"ExchangePyObjectFactory", (PyCFunction) exchange_py_object_factory, METH_VARARGS | METH_KEYWORDS};
}

int main() {

    Py_Initialize();


    PyObject *entry_point = nullptr;
    PyObject *exchange_factory = nullptr;
    PyObject *main_ret = nullptr;

    if (PyType_Ready(&LdH::Studying::BGTU::OOP::Demo::ExchangePyObject::TypeObject))
        goto ERR0;


    if (LdH::Studying::BGTU::OOP::Demo::run_widgets(&entry_point))
        goto ERR0;

    exchange_factory = PyCFunction_New(&LdH::Studying::BGTU::OOP::Demo::exchange_py_object_factory_def, nullptr);
    if (exchange_factory == nullptr)
        goto ERR1;


    main_ret = PyObject_CallOneArg(entry_point, exchange_factory);
    Py_DECREF(exchange_factory);
    Py_DECREF(entry_point);
    if (main_ret == nullptr) {
        goto RET;
    }
    Py_DECREF(main_ret);

    RET:
    return Py_FinalizeEx();

    ERR2:
    Py_DECREF(exchange_factory);
    ERR1:
    Py_DECREF(entry_point);

    ERR0:
    goto RET;
}