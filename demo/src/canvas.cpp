#include <cstdio>
#include <Python.h>
#include <LdH/studying/bgtu/oop/canvas.hpp>
#include "canvas.hpp"
#include "callback_wrappers.hpp"


namespace LdH::Studying::BGTU::OOP::Demo {
    class PythonErrorOccurredException : std::exception {

    };


    class QPainterPathPointsCollector final : public Interface::CanvasLinePointsCollector {
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


    class QPainterPathConfiguration final : public Interface::CanvasConfiguration {
    private:
        PyObject *const path;
        PyObject *const signal;
    public:
        QPainterPathConfiguration(
                PyObject *const path,
                PyObject *const signal,
                const double cw, double ch
        ) : path{path}, signal{signal} {}

        void set_view_area(double min_x, double max_x, double min_y, double max_y) final {
            PyObject *_ret = PyObject_CallMethod(this->path, "set_view_area", "dddd", min_x, min_y, max_x - min_x, max_x - min_y);
            if (_ret == nullptr)
                throw PythonErrorOccurredException();
            else
                Py_DECREF(_ret);
        }

        void draw_line(LdH::Studying::BGTU::OOP::Interface::CanvasLineArtist *drawer) final {
            QPainterPathPointsCollector collector{
                    path
            };
            try {
                drawer->draw_line(&collector);
            } catch (PythonErrorOccurredException &) {
                PyErr_Print();
                PyErr_Clear();
            } catch (std::exception &err) {
                PySys_WriteStderr("%s\n", err.what());
            }

            PyObject *ret = PyObject_CallMethod(this->signal, "emit", "O", this->path == nullptr ? Py_None : this->path);
            if (ret == nullptr)
                throw PythonErrorOccurredException();
        }
    };


    void QPainterPathCanvasProvider::_reconfigure_no_catch(LdH::Studying::BGTU::OOP::Interface::CanvasInitializer *with) const {
        PyObject *path = PyObject_CallNoArgs(this->path_constructor);
        if (path == nullptr)
            throw PythonErrorOccurredException();

        QPainterPathConfiguration cfg{
                path, this->signal, this->cw, this->ch
        };
        try {
            with->configure(&cfg);
            Py_DECREF(path);
        } catch (std::exception &) {
            Py_DECREF(path);
            throw;
        }
    }


    void QPainterPathCanvasProvider::reconfigure(LdH::Studying::BGTU::OOP::Interface::CanvasInitializer *with) const {
        try {
            this->_reconfigure_no_catch(with);
        } catch (PythonErrorOccurredException &) {
            PyErr_Print();
            PyErr_Clear();
        } catch (std::exception &err) {
            PySys_WriteStderr("%s\n", err.what());
        }
    }

}