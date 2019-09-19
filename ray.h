#pragma once
#include "vec3.h"
#include "aabb.h"
#include "float.h"
#include <algorithm>
#include "constant.h"
#include "hittable.h"


class ray {
public:
    vec3 A, B;
    ray() {}
    ray(const vec3& a, const vec3& b) {A=a; B=b;}
    vec3 origin() const {return A;}
    vec3 direction() const {return B;}
    vec3 point_at_parameter(float t) const {return A + B * t;}
    bool hit_bbox(const aabb& bbox, float at_min, float at_max, hit_record& rec) const {
        float t_min = at_min, t_max = at_max;
        for (int i = 0; i < 3; i++) {
            if (std::fabs(B[i]) < 1e-8f) {
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

            if (t1 <= t_min || t0 >= t_max)  {
                return false;
            }

            if (t0 > t_min) {
                t_min = t0;
            }

            t_min = std::fmax(t0, t_min);
            t_max = std::fmin(t1, t_max);
        }

        for (int i = 0; i < 3; i++) {
            if (std::fabs(B[i]) < 1e-8f) {
                if (A[i] > bbox.a[i] && A[i] < bbox.b[i])
                    continue;
                else 
                    return false;
            }

            bool swapped = false;
            float t0 = (bbox.a[i] - A[i]) / B[i];
            float t1 = (bbox.b[i] - A[i]) / B[i];
            if (t0 > t1) {
                std::swap(t0, t1);
                swapped = true;
            }

            if (t0 >= t_min) {
                rec.t = t0;
                rec.p = t0*B + A;
                rec.p[i] = swapped? bbox.b[i] : bbox.a[i];
                vec3 normal(0, 0, 0);
                normal[i] = swapped? 1 : -1;
                rec.normal = normal;
                return true;
            }

            if (t_min == at_min && t1 <= t_max) {
                rec.t = t1;
                rec.p = t1*B + A;
                rec.p[i] = swapped? bbox.a[i] : bbox.b[i];
                vec3 normal(0, 0, 0);
                normal[i] = swapped? -1 : 1;
                rec.normal = normal;
                return true;
            }
        }

        return false;
    }

    bool hit_bbox(float t_min, float t_max, const aabb& bbox) const {
        for (int i = 0; i < 3; i++) {
            if (std::fabs(B[i]) < 1e-8f) {
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

            if (t1 <= t_min || t0 >= t_max)  {
                return false;
            }

            if (t0 > t_min) {
                t_min = t0;
            }

            t_min = std::fmax(t0, t_min);
            t_max = std::fmin(t1, t_max);
        }

        return true;
    }
};