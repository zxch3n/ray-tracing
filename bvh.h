#pragma once
#include "aabb.h"
#include "hittable.h" 
#include "rand.h"
#include <stdlib.h>
#include <functional>
#include <iostream>
#include <vector>


class bvh: public hittable {
public:
    hittable *left, *right;
    aabb box;
    bvh(hittable* left, hittable *right): left(left), right(right){}
    bvh(std::vector<hittable*>& hittables, int l, int r) {
        int n = r - l;
        int m = (r + l) / 2;
        if (n == 1) {
            left = hittables[l];
            right = hittables[l];
            left->bounding_box(box);
            return;
        }

        if (n == 2) {
            left = hittables[l];
            right = hittables[l + 1];
            aabb l_box, r_box;
            left->bounding_box(l_box);
            right->bounding_box(r_box);
            box = merge_aabb(l_box, r_box);
            return;
        }

        int axis = int(random_double() * 3);
        std::sort(hittables.begin(), hittables.end(), [axis](hittable* a, hittable* b) {
            aabb abox, bbox;
            if (!a->bounding_box(abox) || !b->bounding_box(bbox))
            {
                std::cerr << "bbox err";
            }

            if(abox.a[axis] - bbox.a[axis] < 0)
                return true;
            return false;
        });


        left = new bvh(hittables, l, m);
        right = new bvh(hittables, m, r);
        aabb l_box, r_box;
        left->bounding_box(l_box);
        right->bounding_box(r_box);
        box = merge_aabb(l_box, r_box);
        return;
    }

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record & record) const {
        if (!r.hit_bbox(t_min, t_max, box)) {
            return false;
        }

        hit_record left_rec, right_rec;
        bool left_hit = left->hit(r, t_min, t_max, left_rec);
        bool right_hit = right->hit(r, t_min, t_max, right_rec);
        if (!left_hit && !right_hit) {
            return false;
        }

        if (left_hit && right_hit) {
            if (left_rec.t < right_rec.t) {
                record = left_rec;
                return true;
            } else {
                record = right_rec;
                return true;
            }
        }

        if (right_hit){
            record = right_rec;
            return true;
        }
        
        if (left_hit) {
            record = left_rec;
            return true;
        }

        return false;
    }

    bool bounding_box(aabb& bbox) const {
        bbox = box;
        return true;
    }
};