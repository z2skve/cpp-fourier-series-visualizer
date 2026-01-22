#include "math_engine.h"
#include "exprtk.hpp"

namespace ui {

struct MathParser::Impl {
    float x_var = 0.0f;
    exprtk::symbol_table<float> symbol_table;
    exprtk::expression<float> expression;
    exprtk::parser<float> parser;

    Impl() {
        symbol_table.add_variable("x", x_var);
        symbol_table.add_constants();
        expression.register_symbol_table(symbol_table);
    }
};

MathParser::MathParser() : pimpl_(new Impl()) {}
MathParser::~MathParser() { delete pimpl_; }

bool MathParser::Compile(const std::string& formula) {
    return pimpl_->parser.compile(formula, pimpl_->expression);
}

float MathParser::Evaluate(float x) {
    pimpl_->x_var = x;
    return pimpl_->expression.value();
}

std::function<float(float)> MathParser::GetTargetFunction() {
    return [this](float x) -> float {
        return this->Evaluate(x);
    };
}

} // namespace ui