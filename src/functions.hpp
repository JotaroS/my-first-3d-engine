#pragma once

float linear (float t);
float easeInOutExponential (float t);
float easeOutCubic (float t);
float easeInOutCubic (float t);
float easeOutBack (float t);
float easeInOutBack (float t);
float easeOutElastic (float t);
float easeInOutElastic (float t);
float easeOutBounce (float t);
float easeInOutBounce (float t);
float blink(float t);
float none (float t);
enum class TransitionFunction
{
    None,
    Linear,
    EaseInOutExponential,
    EaseOutCubic,
    EaseInOutCubic,
    EaseOutBack,
    EaseInOutBack,
    EaseOutElastic,
    EaseInOutElastic,
    EaseOutBounce,
    EaseInOutBounce,
    blink,
};

float getRatio(float t, TransitionFunction func);