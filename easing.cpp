#include <cmath>
#include "easing.h"
#define PI 3.1415926545

float easeInSine(float t) {
    return sin(PI / 2.0f * t);
}

float easeOutSine(float t) {
    return 1 + sin(PI * (--t));
}

float easeInOutSine(float t) {
    return 0.5 * (1 + sin(PI * (t - 0.5)));
}

float easeInQuad(float t) {
    return t * t;
}

float easeOutQuad(float t) {
    return t * (2 - t);
}

float easeInOutQuad(float t) {
    return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
}

float easeInCubic(float t) {
    return t * t * t;
}

float easeOutCubic(float t) {
    return 1 + (--t) * t * t;
}

float easeInOutCubic(float t) {
    return t < 0.5 ? 4 * t * t * t : 1 + (--t) * (2 * (--t)) * (2 * t);
}

float easeInQuart(float t) {
    t *= t;
    return t * t;
}

float easeOutQuart(float t) {
    t = (--t) * t;
    return 1 - t * t;
}

float easeInOutQuart(float t) {
    if (t < 0.5) {
        t *= t;
        return 8 * t * t;
    }
    else {
        t = (--t) * t;
        return 1 - 8 * t * t;
    }
}

float easeInExpo(float t) {
    return (pow(2, 8 * t) - 1) / 255;
}

float easeOutExpo(float t) {
    return 1 - pow(2, -8 * t);
}

float easeInOutExpo(float t) {
    if (t < 0.5) {
        return (pow(2, 16 * t) - 1) / 510;
    }
    else {
        return 1 - 0.5 * pow(2, -16 * (t - 0.5));
    }
}

float easeInElastic(float t) {
    float t2 = t * t;
    return t2 * t2 * sin(t * PI * 4.5);
}

float easeOutElastic(float t) {
    float t2 = (t - 1) * (t - 1);
    return 1 - t2 * t2 * cos(t * PI * 4.5);
}

float easeInOutElastic(float t) {
    float t2;
    if (t < 0.45) {
        t2 = t * t;
        return 8 * t2 * t2 * sin(t * PI * 9);
    }
    else if (t < 0.55) {
        return 0.5 + 0.75 * sin(t * PI * 4);
    }
    else {
        t2 = (t - 1) * (t - 1);
        return 1 - 8 * t2 * t2 * sin(t * PI * 9);
    }
}

float easeInBounce(float t) {
    return pow(2, 6 * (t - 1)) * abs(sin(t * PI * 3.5));
}

float easeOutBounce(float t) {
    return 1 - pow(2, -6 * t) * abs(cos(t * PI * 3.5));
}

float easeInOutBounce(float t) {
    if (t < 0.5) {
        return 8 * pow(2, 8 * (t - 1)) * abs(sin(t * PI * 7));
    }
    else {
        return 1 - 8 * pow(2, -8 * t) * abs(sin(t * PI * 7));
    }
}