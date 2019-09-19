#pragma once
#include "hittable.h"
#include "texture.h"
#include "material.h"


class cuboid: public hittable {
public:
    cuboid() {}
    cuboid(vec3 center, vec3 size, material* _m) {
        m = _m;
        _box = aabb(center - size/2, center + size/2);
    }
    cuboid(aabb& box, material* m): _box(box), m(m) {}

    static cuboid* make_xy_face(const vec3& center, float x_size, float y_size, material* m) {
        float MIN_F = 1e-7;
        vec3 v(x_size, y_size, MIN_F);
        return new cuboid(center, v, m);
    }

    virtual bool hit(const ray& r, float t_min, 
            float t_max, hit_record& record) const;
    virtual bool bounding_box(aabb& box) const;
    aabb _box;
    material *m;

private:
    vec3 get_normal(vec3& p, const int axis, const bool hit_a) const {
        vec3 ans(0, 0, 0);
        if (hit_a)
        {
            p[axis] = _box.a[axis];
            ans[axis] = -1.0f;
        }
        else
        {
            p[axis] = _box.b[axis];
            ans[axis] = 1.0f;
        }
        return ans;
    }
};

bool cuboid::hit(const ray &r, float t_min,
                 float t_max, hit_record &record) const
{
    if (r.hit_bbox(_box, t_min, t_max, record)) {
        record.m = m;
        record.u = 0; // TODO
        record.v = 0;
        return true;
    } else {
        return false;
    }
}

bool cuboid::bounding_box(aabb& box) const {
    box = _box;
    return true;
}