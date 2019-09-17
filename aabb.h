#pragma once
#include "vec3.h"

inline float min(float a, float b) {
    return a > b? b : a;
}

inline float max(float a, float b) {
    return a < b? b : a;
}


class aabb {
public:
    vec3 a, b;
    aabb () {}
    aabb (const vec3& a, const vec3& b): a(a), b(b) {}
    static aabb make_aabb(const vec3& a, const vec3& b) {
        return aabb(
            vec3(min(a.x(), b.x()), min(a.y(), b.y()), min(a.z(), b.z())),
            vec3(max(a.x(), b.x()), max(a.y(), b.y()), max(a.z(), b.z()))
        );
    }
};

aabb merge_aabb(const aabb &box0, const aabb &box1)
{
    vec3 small(min(box0.a.x(), box1.a.x()),
               min(box0.a.y(), box1.a.y()),
               min(box0.a.z(), box1.a.z()));
    vec3 big(max(box0.b.x(), box1.b.x()),
             max(box0.b.y(), box1.b.y()),
             max(box0.b.z(), box1.b.z()));
    return aabb(small, big);
}