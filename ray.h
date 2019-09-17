#pragma once
#include "vec3.h"
#include "aabb.h"
#include <algorithm>


class ray {
public:
    vec3 A, B;
    ray() {}
    ray(const vec3& a, const vec3& b) {A=a; B=b;}
    vec3 origin() const {return A;}
    vec3 direction() const {return B;}
    vec3 point_at_parameter(float t) const {return A + B * t;}
    bool hit_bbox(float t_min, float t_max, const aabb& bbox) const {
        for (int i = 0; i < 3; i++) {
            if (B[i] > -1e-7 && B[i] < 1e-7) {
                if (A[i] > bbox.a[i] && A[i] < bbox.b[i])
                    continue;
                else 
                    return false;
            }

            float t0 = (bbox.a[i] - A[i]) / B[i];
            float t1 = (bbox.b[i] - A[i]) / B[i];
            if (t0 > t1) {
                std::swap(t0, t1);
            }

            if (t1 < t_min || t0 > t_max)  {
                return false;
            }

            t_min = std::max(t0, t_min);
            t_max = std::min(t1, t_max);
        }

        return true;
    }
};