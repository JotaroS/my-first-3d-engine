#include "functions.hpp"
#include <cmath>
float simplePow(float base, int exp)
{
    float result = 1.0f;
    for (int i = 0; i < exp; ++i)
    {
        result *= base;
    }
    return result;
}

float linear (float t)
{
    return t;
}

float easeInOutExponential (float t)
{
    if (t < 0.5f)
        return 0.5f * std::pow(2.0f, (20.0f * t) - 10.0f);
    else
        return -0.5f * std::pow(2.0f, (-20.0f * t) + 10.0f) + 1.0f;
}

float easeOutCubic (float t)
{
    t -= 1.0f;
    return t * t * t + 1.0f;
}

float easeInOutCubic (float t)
{
    if (t < 0.5f)
    {
        return 4.0f * t * t * t;
    }
    else
    {
        t -= 1.0f;
        return 4.0f * t * t * t + 1.0f;
    }
}


float easeInOutBack (float t)
{
    float c1 = 1.70158f;
    float c2 = c1 * 1.525f;

    if (t < 0.5f)
    {
        return (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f;
    }
    else
    {
        t = t * 2.0f - 2.0f;
        return (std::pow(t, 2.0f) * ((c2 + 1.0f) * t + c2) + 2.0f) / 2.0f;
    }
}

float easeOutElastic (float t)
{
    float c4 = (2.0f * 3.14159265f) / 3.0f;

    if (t == 0.0f)
        return 0.0f;
    if (t == 1.0f)
        return 1.0f;

    return std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
}

float easeInOutElastic (float t)
{
    float c5 = (2.0f * 3.14159265f) / 4.5f;

    if (t == 0.0f)
        return 0.0f;
    if (t == 1.0f)
        return 1.0f;

    if (t < 0.5f)
    {
        return -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f;
    }
    else
    {
        return (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
    }
}

float easeOutBounce (float t)
{
    float n1 = 7.5625f;
    float d1 = 2.75f;

    if (t < 1.0f / d1)
    {
        return n1 * t * t;
    }
    else if (t < 2.0f / d1)
    {
        t -= 1.5f / d1;
        return n1 * t * t + 0.75f;
    }
    else if (t < 2.5f / d1)
    {
        t -= 2.25f / d1;
        return n1 * t * t + 0.9375f;
    }
    else
    {
        t -= 2.625f / d1;
        return n1 * t * t + 0.984375f;
    }
}

float easeInOutBounce (float t)
{
    if (t < 0.5f)
    {
        return (1.0f - easeOutBounce(1.0f - 2.0f * t)) / 2.0f;
    }
    else
    {
        return (1.0f + easeOutBounce(2.0f * t - 1.0f)) / 2.0f;
    }
}

float easeOutBack(float t)
{
    float c1 = 1.70158f;
    float c3 = c1 + 1.0f;

    t -= 1.0f;
    return 1.0f + c3 * t * t * t + c1 * t * t;
}

float blink(float t)
{
    t = fminf(fmaxf(t, 0.0f), 1.0f);
    // Quick exponential rise (cubic), smooth sine fall
    if (t < 0.3f)
    {
        // fast up: map [0, 0.3] -> [0, 1]
        float x = t / 0.3f;
        return x * x * (3.0f - 2.0f * x); // ease-out cubic
    }
    else
    {
        // smooth fall: map [0.3, 1] -> [1, 0]
        float x = (t - 0.3f) / 0.7f;
        return cosf(x * 3.14159265f * 0.5f); // smooth cosine down
    }
}

float none (float t)
{
    return 1.0f;
}

float getRatio(float t, TransitionFunction func)
{
    switch (func)
    {
        case TransitionFunction::None:
            return 1.0f;
        case TransitionFunction::Linear:
            return linear(t);
        case TransitionFunction::EaseInOutExponential:
            return easeInOutExponential(t);
        case TransitionFunction::EaseOutCubic:
            return easeOutCubic(t);
        case TransitionFunction::EaseInOutCubic:
            return easeInOutCubic(t);
        case TransitionFunction::EaseOutBack:
            return easeOutBack(t);
        case TransitionFunction::EaseInOutBack:
            return easeInOutBack(t);
        case TransitionFunction::EaseOutElastic:
            return easeOutElastic(t);
        case TransitionFunction::EaseInOutElastic:
            return easeInOutElastic(t);
        case TransitionFunction::EaseOutBounce:
            return easeOutBounce(t);
        case TransitionFunction::EaseInOutBounce:
            return easeInOutBounce(t);
        case TransitionFunction::blink:
            return blink(t);

        default:
            return 1.0f;
    }
}