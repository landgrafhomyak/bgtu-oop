#pragma once

#include <exception>
#include <string>
#include "expression.hpp"

namespace LdH::Studying::BGTU::OOP::Interface {
    // runtime side
    class CanvasPlot {
    public:
        virtual void addPoint(double x, double y) = 0;
    };

    // app side
    class CanvasLineArtist {
    public:
        virtual void draw_line(CanvasPlot *collector) const = 0;
    };

    // runtime side
    class CanvasProvider {
    public:
        virtual void reconfigure(double min_x, double max_x, double min_y, double max_y, CanvasLineArtist const *with) = 0;
    };
}