#pragma once
#include "ray.h"
#include "aabb.h"


class material;
struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    material* m;
};


class hittable {
public:
    virtual bool hit(const ray& r, float t_min, 
            float t_max, hit_record& record) const = 0;
    virtual bool bounding_box(aabb& box) const = 0;
};