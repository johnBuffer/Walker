#pragma once
#include <cmath>


namespace nt
{

using ActivationPtr = float (*)(float);

enum class Activation : uint8_t
{
    None,
    Sigm,
    Relu,
    Tanh,
};

struct ActivationFunction
{
    static ActivationPtr getFunction(Activation activation)
    {
        switch (activation) {
            case Activation::None:
                return none;
            case Activation::Sigm:
                return sigm;
            case Activation::Relu:
                return relu;
            case Activation::Tanh:
                return tanh;
            default:
                return none;
        }
    }

    static float none(float x)
    {
        return x;
    }

    static float sigm(float x)
    {
        return 1.0f / (1.0f + std::exp(-4.5f * x));
    }

    static float relu(float x)
    {
        return (x + std::abs(x)) * 0.5f;
    }

    static float tanh(float x)
    {
        return std::tanh(x);
    }
};

}