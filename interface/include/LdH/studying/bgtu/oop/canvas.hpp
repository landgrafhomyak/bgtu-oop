#pragma once

#include <exception>
#include <string>
#include "expression.hpp"

namespace LdH::Studying::BGTU::OOP::Interface {
    // runtime side
    class CanvasLinePointsCollector {
    public:
        virtual void addPoint(double x, double y) = 0;
    };

    // app side
    class CanvasLineArtist {
    public:
        virtual void draw_line(CanvasLinePointsCollector *collector) = 0;
    };

    // runtime side
    class CanvasConfiguration {
    public:
        virtual void set_view_area(double min_x, double max_x, double min_y, double max_y) = 0;

        virtual void draw_line(CanvasLineArtist *drawer) = 0;
    };

    // app side
    class CanvasInitializer {
    public:
        virtual void configure(CanvasConfiguration *config) = 0;
    };

    // runtime side
    class CanvasProvider {
    public:
        virtual void reconfigure(CanvasInitializer *with) const = 0;
    };
}