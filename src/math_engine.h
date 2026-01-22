#ifndef MATH_ENGINE_H_
#define MATH_ENGINE_H_

#include <string>
#include <functional>

namespace ui {
    class MathParser {
    public:
        MathParser();
        ~MathParser();

        bool Compile(const std::string& formula);
        float Evaluate(float x);

        std::function<float(float)> GetTargetFunction();

    private:
        struct Impl;
        Impl* pimpl_;
    };
}

#endif