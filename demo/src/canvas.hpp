#pragma once

#include <Python.h>
#include <LdH/studying/bgtu/oop/canvas.hpp>

namespace LdH::Studying::BGTU::OOP::Demo {
    class QPainterPathCanvasProvider final : public Interface::CanvasProvider {
    private:
        PyObject *const path_constructor;
        PyObject *const signal;

    public:
        double cw, ch;
        inline QPainterPathCanvasProvider(
                PyObject *const path_constructor,
                PyObject *const signal
        ) : path_constructor{path_constructor}, signal{signal}, cw{0}, ch{0} {}

    public:
        void reconfigure(double min_x, double max_x, double min_y, double max_y, Interface::CanvasLineArtist const * with) final;
    };
}
