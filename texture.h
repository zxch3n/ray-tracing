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


class image_texture: public texture {
    public:
        image_texture() {}
        image_texture(unsigned char *pixels, int A, int B)
            : data(pixels), nx(A), ny(B) {}
        virtual vec3 value(float u, float v, const vec3& p) ;
        unsigned char *data;
        int nx, ny;
};


vec3 image_texture::value(float u, float v, const vec3& p) {
     int i = (  u) * nx;
     int j = (1-v) * ny - 0.001;
     if (i < 0) i = 0;
     if (j < 0) j = 0;
     if (i > nx-1) i = nx-1;
     if (j > ny-1) j = ny-1;
     float r = int(data[3*i + 3*nx*j]  ) / 255.0;
     float g = int(data[3*i + 3*nx*j+1]) / 255.0;
     float b = int(data[3*i + 3*nx*j+2]) / 255.0;
     return vec3(r, g, b);
}