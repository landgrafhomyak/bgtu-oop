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


            class CanvasInitializerImpl final : public Interface::CanvasInitializer {
            private:
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

                    void draw_line(Interface::CanvasLinePointsCollector *collector) final {
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

            public:
                double x_center;
                double x_radius;
                double y_center;
                double y_radius;
                Interface::Expression const *expr;
            private:
                Interface::ExpressionContext *const ctx;
                const unsigned steps_count;
            public:
                CanvasInitializerImpl(
                        Interface::ApplicationCallbacks::ApplicationFactory::view_area_settings view_area_settings,
                        Interface::ExpressionContext *ctx,
                        unsigned steps_count
                ) : x_center{(view_area_settings.max_x + view_area_settings.min_x) / 2},
                    x_radius{(view_area_settings.max_x - view_area_settings.min_x) / 2},
                    y_center{(view_area_settings.max_y + view_area_settings.min_y) / 2},
                    y_radius{(view_area_settings.max_y - view_area_settings.min_y) / 2},
                    expr{nullptr},
                    ctx{ctx},
                    steps_count{steps_count} {}

                void configure(Interface::CanvasConfiguration *config) final {
                    double x_start = this->x_center - this->x_radius;
                    double x_end = this->x_center + this->x_radius;
                    config->set_view_area(
                            x_start,
                            x_end,
                            this->y_center - this->y_radius,
                            this->y_center + this->y_radius
                    );
                    if (this->expr != nullptr) {
                        LineArtistImpl artist{
                                this->expr,
                                this->ctx,
                                x_start,
                                x_end,
                                this->steps_count
                        };
                        config->draw_line(&artist);
                    }
                }
            };


            ExpressionContextImpl expr_ctx;
            CanvasInitializerImpl cnv_init;
            Interface::CanvasProvider const *canvas;
            Interface::ExpressionParser const *parser;
            Interface::ApplicationCallbacks::ApplicationFactory::scroll_and_zoom_settings scroll_and_zoom_settings;

            DefaultApplicationLogic(
                    Interface::CanvasProvider const *canvas,
                    Interface::ExpressionParser const *parser,
                    unsigned int quality_steps_count,
                    Interface::ApplicationCallbacks::ApplicationFactory::view_area_settings initial_view_area,
                    Interface::ApplicationCallbacks::ApplicationFactory::scroll_and_zoom_settings scroll_and_zoom_settings
            ) : expr_ctx{}, cnv_init{initial_view_area, &this->expr_ctx, quality_steps_count}, canvas{canvas}, parser{parser}, scroll_and_zoom_settings{scroll_and_zoom_settings} {}


            void redraw() {
                this->canvas->reconfigure(&this->cnv_init);
            }

        public:
            class Factory final : public Interface::ApplicationCallbacks::ApplicationFactory {
            public:
                Factory() = default;


                Interface::ApplicationCallbacks *create(
                        Interface::CanvasProvider const *canvas,
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

            void scrollX(signed steps) final {
                this->cnv_init.x_center += this->cnv_init.x_radius * steps * this->scroll_and_zoom_settings.scroll_x_step;
                this->redraw();
            }

            void scrollY(signed steps) final {
                this->cnv_init.y_center += this->cnv_init.y_radius * steps * this->scroll_and_zoom_settings.scroll_y_step;
                this->redraw();
            }

            void zoomX(signed steps) final {
                this->cnv_init.x_radius /= std::pow(this->scroll_and_zoom_settings.zoom_x_step, steps);
                this->redraw();

            }

            void zoomY(signed steps) final {
                this->cnv_init.y_radius /= std::pow(this->scroll_and_zoom_settings.zoom_y_step, steps);
                this->redraw();
            }

            void inputExpression(const char *raw) final {
                try {
                    this->cnv_init.expr = this->parser->parse(raw);
                } catch (std::exception &) {
                    this->cnv_init.expr = nullptr;
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
        };

        static DefaultApplicationLogic::Factory const _default_application_logic{};
    }

    Interface::ApplicationCallbacks::ApplicationFactory const *const default_application_logic = &DefaultApplicationLogic::_default_application_logic;
}
