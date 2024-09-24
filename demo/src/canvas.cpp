#include <cstdio>
#include <new>
#include <Python.h>
#include <LdH/studying/bgtu/oop/canvas.hpp>
#include "canvas.hpp"
#include "callback_wrappers.hpp"


namespace LdH::Studying::BGTU::OOP::Demo {
    class PythonErrorOccurredException : public std::exception {

    };

    class QPainterPathPointsCollector final : public Interface::CanvasPlot {
    private:
        PyObject *const path;
    public:
        explicit QPainterPathPointsCollector(
                PyObject *path
        ) : path{path} {}

        void addPoint(double x, double y) final {
            PyObject *ret = PyObject_CallMethod(this->path, "add_point", "dd", x, y);
            if (ret == nullptr) {
                throw PythonErrorOccurredException();
            } else {
                Py_DECREF(ret);
            }
        }
    };

    void QPainterPathCanvasProvider::reconfigure(double min_x, double max_x, double min_y, double max_y, Interface::CanvasLineArtist const *with) {
        PyObject *path, *_ret, *emit_p;
        QPainterPathPointsCollector cfg{nullptr};

        path = PyObject_CallNoArgs(this->path_constructor);
        if (path == nullptr) {
            PyErr_Print();
            PyErr_Clear();
            goto EMIT_NONE;
        }
        new(&cfg)QPainterPathPointsCollector{path};

        _ret = PyObject_CallMethod(path, "set_view_area", "dddd", min_x, min_y, max_x - min_x, max_x - min_y);
        if (_ret == nullptr)
            throw PythonErrorOccurredException();
        else
            Py_DECREF(_ret);

        try {
            with->draw_line(&cfg);
            emit_p = path;
        } catch (PythonErrorOccurredException &) {
            PyErr_Print();
            PyErr_Clear();
            goto EMIT_NONE;
        } catch (std::exception &e) {
            Py_DECREF(path);
            PySys_WriteStderr("%s\n", e.what());
            goto EMIT_NONE;

        }

        EMIT:
        _ret = PyObject_CallMethod(this->signal, "emit", "O", emit_p);
        if (_ret == nullptr) {
            PyErr_Print();
            PyErr_Clear();
        } else {
            Py_DECREF(_ret);
        }
        return;

        EMIT_NONE:
        Py_INCREF(Py_None);
        emit_p = Py_None;
        goto EMIT;
    }

}