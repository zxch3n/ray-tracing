#pragma once
#include "hittable.h"


class hittable_list: public hittable {
public:
    hittable_list() {}
    hittable_list(hittable **list, int list_size): 
        list(list), list_size(list_size) {}
    virtual bool hit(const ray& r, float t_min, 
            float t_max, hit_record& record) const;
    hittable **list;
    int list_size;
};


bool hittable_list::hit(const ray& r, float t_min, 
            float t_max, hit_record& record) const {
    hit_record temp_record;
    bool hit_anything = false;
    double closest = t_max;
    for (int i = 0; i < list_size; i++) {
        if (list[i]->hit(r, t_min, t_max, temp_record)){
            hit_anything = true;
            if (temp_record.t < closest) {
                closest = temp_record.t;
                record = temp_record;
            }
        }
    }

    return hit_anything;
}