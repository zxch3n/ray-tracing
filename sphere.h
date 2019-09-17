#pragma once
#include <iostream>
#include "material.h"
#include "hittable.h"

class sphere : public hittable
{
public:
    vec3 center;
    float radius;
    sphere(){};
    sphere(vec3 center, float radius, material *m) : center(center), radius(radius), m(m)
    {
        bbox = aabb(
            center - vec3(radius, radius, radius), 
            center + vec3(radius, radius, radius)
        );
    };

    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &record) const;
    virtual bool bounding_box(aabb & box) const
    {
        box = bbox;
        return true;
    }

    material* m;
    aabb bbox;
};

bool sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const
{
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    if (discriminant > 0)
    {
        float temp = (-b - sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.m = m;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.m = m;
            return true;
        }
    }
    return false;
}