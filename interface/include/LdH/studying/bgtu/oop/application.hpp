#pragma once

#include <exception>
#include <string>
#include "parser.hpp"
#include "canvas.hpp"

namespace LdH::Studying::BGTU::OOP::Interface {
    class ApplicationCallbacks {
    public:
        virtual void scrollX(signed steps) = 0;

        virtual void scrollY(signed steps) = 0;

        virtual void zoomX(signed steps) = 0;

        virtual void zoomY(signed steps) = 0;

        virtual void inputExpression(char const *raw) = 0;

        virtual void setParam(char const *name, double value) = 0;

        virtual void removeParam(char const *name) = 0;

        virtual ~ApplicationCallbacks() = 0;

        class ApplicationFactory {
        public:
            struct scroll_and_zoom_settings {
                double scroll_x_step;
                double scroll_y_step;
                double zoom_x_step;
                double zoom_y_step;
            };

            struct view_area_settings {
                double min_x;
                double max_x;
                double min_y;
                double max_y;
            };

            static constexpr scroll_and_zoom_settings DEFAULT_SCROLL_AND_ZOOM_SETTINGS{
                    .scroll_x_step = 0.1,
                    .scroll_y_step = 0.1,
                    .zoom_x_step = 1.3,
                    .zoom_y_step = 1.3
            };

            static constexpr view_area_settings DEFAULT_VIEW_AREA_SETTINGS{
                    .min_x = -10,
                    .max_x = 10,
                    .min_y = -10,
                    .max_y = 10
            };

            virtual ApplicationCallbacks *create(
                    CanvasProvider const *canvas,
                    ExpressionParser const *parser,
                    unsigned quality_steps_count,
                    view_area_settings initial_view_area,
                    scroll_and_zoom_settings scroll_and_zoom_settings
            ) const = 0;
        };
    };


}