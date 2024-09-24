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

    private:
        void _reconfigure_no_catch(LdH::Studying::BGTU::OOP::Interface::CanvasInitializer *with) const;
    public:
        void reconfigure(LdH::Studying::BGTU::OOP::Interface::CanvasInitializer *with) const final;
    };
}
