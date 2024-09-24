#include <cmath>
#include <utility>
#include <string>
#include <map>
#include <exception>
#include <LdH/studying/bgtu/oop/expression.hpp>
#include <LdH/studying/bgtu/oop/parser.hpp>
#include <LdH/studying/bgtu/oop/canvas.hpp>
#include <LdH/studying/bgtu/oop/application.hpp>
#include <LdH/studying/bgtu/oop/default_application_logic.hpp>

namespace LdH::Studying::BGTU::OOP {
    namespace DefaultApplicationLogic {
        class DefaultApplicationLogic final : public Interface::ApplicationCallbacks {
        private:
            class ExpressionContextImpl final : public Interface::ExpressionContext {
            public:
                std::map<std::string, double> params{};

                ExpressionContextImpl() = default;

                [[nodiscard]] double get_parameter(const std::string &name) const final {
                    auto it = this->params.find(name);
                    if (it == this->params.end())
                        throw ParameterNotFoundException(name);
                    return it->second;
                }
            };

            class LineArtistImpl final : public Interface::CanvasLineArtist {
            private:
                class ExpressionContextWithX final : Interface::ExpressionContext {
                public:
                    double x;
                private:
                    Interface::ExpressionContext const *const real;
                public:
                    explicit ExpressionContextWithX(Interface::ExpressionContext const *real) : real{real} {}

                    [[nodiscard]] double get_parameter(const std::string &name) const final {
                        if (name == "x")
                            return this->x;
                        else
                            return this->real->get_parameter(name);
                    }
                };

                Interface::Expression const *const expr;
                Interface::ExpressionContext const *const in_ctx;
                double const x_start;
                double const x_end;
                unsigned steps_count;


            public:
                LineArtistImpl(
                        Interface::Expression const *const expr,
                        Interface::ExpressionContext const *const in_ctx,
                        const double x_start,
                        const double x_end,
                        unsigned int steps_count
                ) : expr{expr}, in_ctx{in_ctx}, x_start{x_start}, x_end{x_end}, steps_count{steps_count} {}

                void draw_line(Interface::CanvasPlot *collector) const final {
                    double step = (this->x_end - this->x_start) / this->steps_count;
                    double x = this->x_start;
                    ExpressionContextWithX ctx_x{this->in_ctx};

                    for (; x <= this->x_end; x += step) {
                        ctx_x.x = x;
                        const double y = this->expr->calculate((Interface::ExpressionContext *) &ctx_x);
                        collector->addPoint(x, y);
                    }
                }
            };

            class NoopLineArtist final : public Interface::CanvasLineArtist {
            private:
                NoopLineArtist() = default;

            public:
                void draw_line(Interface::CanvasPlot *collector) const final {}

                static const NoopLineArtist INSTANCE;
            };


            ExpressionContextImpl expr_ctx;
            Interface::CanvasProvider *canvas;
            Interface::ExpressionParser const *parser;
            Interface::ApplicationCallbacks::ApplicationFactory::scroll_and_zoom_settings scroll_and_zoom_settings;
            struct {
                double x_center;
                double x_radius;
                double y_center;
                double y_radius;
            } view_area;
            Interface::Expression *expr;
            unsigned quality_steps_count;

            DefaultApplicationLogic(
                    Interface::CanvasProvider *canvas,
                    Interface::ExpressionParser const *parser,
                    unsigned quality_steps_count,
                    Interface::ApplicationCallbacks::ApplicationFactory::view_area_settings initial_view_area,
                    Interface::ApplicationCallbacks::ApplicationFactory::scroll_and_zoom_settings scroll_and_zoom_settings
            ) : expr_ctx{}, canvas{canvas}, parser{parser}, scroll_and_zoom_settings{scroll_and_zoom_settings}, view_area{
                    .x_center = (initial_view_area.max_x + initial_view_area.min_x) / 2,
                    .x_radius = (initial_view_area.max_x - initial_view_area.min_x) / 2,
                    .y_center  = (initial_view_area.max_y + initial_view_area.min_y) / 2,
                    .y_radius = (initial_view_area.max_y - initial_view_area.min_y) / 2,
            }, quality_steps_count{quality_steps_count}, expr{nullptr} {}


            void redraw() {
                LineArtistImpl artist{
                        this->expr,
                        &this->expr_ctx,
                        this->view_area.x_center - this->view_area.x_radius,
                        this->view_area.x_center + this->view_area.x_radius,
                        this->quality_steps_count
                };

                this->canvas->reconfigure(
                        this->view_area.x_center - this->view_area.x_radius,
                        this->view_area.x_center + this->view_area.x_radius,
                        this->view_area.y_center - this->view_area.y_radius,
                        this->view_area.y_center + this->view_area.y_radius,
                         (this->expr == nullptr ? (Interface::CanvasLineArtist const *) &NoopLineArtist::INSTANCE : &artist)
                );
            }

        public:
            void scrollX(signed steps) final {
                this->view_area.x_center += this->view_area.x_radius * steps * this->scroll_and_zoom_settings.scroll_x_step;
                this->redraw();
            }

            void scrollY(signed steps) final {
                this->view_area.y_center += this->view_area.y_radius * steps * this->scroll_and_zoom_settings.scroll_y_step;
                this->redraw();
            }

            void zoomX(signed steps) final {
                this->view_area.x_radius /= std::pow(this->scroll_and_zoom_settings.zoom_x_step, steps);
                this->redraw();

            }

            void zoomY(signed steps) final {
                this->view_area.y_radius /= std::pow(this->scroll_and_zoom_settings.zoom_y_step, steps);
                this->redraw();
            }

            void inputExpression(const char *raw) final {
                try {
                    this->expr = this->parser->parse(raw);
                } catch (std::exception &) {
                    this->expr = nullptr;
                    this->redraw();
                    throw;
                }
                this->redraw();
            }

            void setParam(const char *name, double value) final {
                this->expr_ctx.params[std::move(std::string{name})] = value;
                this->redraw();
            }

            void removeParam(const char *name) final {
                this->expr_ctx.params.erase(name);
                this->redraw();
            }

            ~DefaultApplicationLogic() final {
//                this->expr_ctx.~ExpressionContextImpl();
//                this->cnv_init.~CanvasInitializerImpl();
                operator delete(this);
            }

            class Factory final : public Interface::ApplicationCallbacks::ApplicationFactory {
            public:
                Factory() = default;


                Interface::ApplicationCallbacks *create(
                        Interface::CanvasProvider *canvas,
                        Interface::ExpressionParser const *parser,
                        unsigned int quality_steps_count,
                        Interface::ApplicationCallbacks::ApplicationFactory::view_area_settings initial_view_area,
                        Interface::ApplicationCallbacks::ApplicationFactory::scroll_and_zoom_settings scroll_and_zoom_settings
                ) const final {
                    return (Interface::ApplicationCallbacks *) (new DefaultApplicationLogic{
                            canvas, parser, quality_steps_count, initial_view_area, scroll_and_zoom_settings
                    });
                }
            };
        };


        const DefaultApplicationLogic::NoopLineArtist DefaultApplicationLogic::NoopLineArtist::INSTANCE{};

        static DefaultApplicationLogic::Factory const _default_application_logic{};
    }

    Interface::ApplicationCallbacks::ApplicationFactory const *const default_application_logic = &DefaultApplicationLogic::_default_application_logic;
}
