#pragma once
#include "ray.h"
const double PI = 3.141592653589793238463;
const float PI_F = 3.14159265358979f;

class camera
{
public:
    camera(vec3 lookfrom, vec3 lookat, vec3 vup,
           float aperture, float vfov, float aspect)
    {
        // vfov is top to bottom in degrees
        radius = aperture / 2;
        float theta = vfov * PI / 180;
        float half_height = tan(theta / 2);
        float half_width = aspect * half_height;
        origin = lookfrom;
        float dist = (lookfrom - lookat).length();
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        lower_left_corner = origin - half_width * u * dist - half_height * v * dist - w * dist;
        horizontal = 2 * half_width * u * dist;
        vertical = 2 * half_height * v * dist;
    }

    ray get_ray(float s, float t)
    {
        vec3 random = radius * random_in_unit_sphere();
        vec3 offset = u * random[1] + v * random[2];
        return ray(origin + offset,
                   lower_left_corner + s * horizontal + t * vertical - origin - offset);
    }

    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    float radius;
};