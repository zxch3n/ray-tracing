#pragma once
#include "vec3.h"
#include <math.h>

class texture {
public:
    virtual vec3 value(float u, float v, const vec3& p) = 0;
};


class constant_texture: public texture {
public:
    vec3 _value;
    constant_texture(const vec3& color): _value(color){}
    virtual vec3 value(float u, float v, const vec3& p) { return _value; }
};


class checker_texture: public texture {
public:
    texture *a_value, *b_value;
    checker_texture(texture* a, texture* b) {a_value = a; b_value = b;}
    virtual vec3 value(float u, float v, const vec3& p) {
        float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0.0) 
            return a_value->value(u, v, p);

        return b_value->value(u, v, p);
    }
};