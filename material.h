#pragma once
#include "ray.h"
#include "hittable.h"
#include "rand.h"
#include "texture.h"


class material {
public:
    virtual bool scatter (const ray& r, hit_record& rec, vec3& attenuation, 
        ray& scattered) const = 0;
};


class lambertian: public material {
public: 
    texture* a;
    lambertian (texture* a): a(a){}
    virtual bool scatter (const ray& r, hit_record& rec, 
        vec3& attenuation, 
        ray& scattered) const;
};

bool lambertian::scatter(
    const ray &r, hit_record &rec,
    vec3 &attenuation, ray &scattered) const
{
    const vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    attenuation = a->value(0, 0, rec.p);
    scattered = ray(rec.p, target - rec.p);
    return true;
}

inline vec3 reflect(const vec3 &dir, const vec3 &normal)
{
    return dir - 2 * dot(normal, dir) * normal;
}

class metal: public material {
public:
    vec3 a;
    float fuzz;
    metal (const vec3&a, const float _fuzz ): a(a){
        if (_fuzz < 1) {
            fuzz = _fuzz;
        } else {
            fuzz = 1;
        }
    }
    virtual bool scatter (const ray& r, hit_record& rec, 
        vec3& attenuation, 
        ray& scattered) const;
};

bool metal::scatter(
    const ray &r, hit_record &rec,
    vec3 &attenuation, ray &scattered) const
{
    vec3 reflect_dir = reflect(r.direction(), rec.normal);
    scattered = ray(rec.p, reflect_dir + fuzz*random_in_unit_sphere());
    attenuation = a;
    return dot(reflect_dir, rec.normal) > 0;
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }

    return false;
}

float schlick(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}


class dielectric: public material {
public:
    float ref_idx;
    dielectric(float ri): ref_idx(ri) {}
    virtual bool scatter(
        const ray &r, hit_record &rec,
        vec3 &attenuation,
        ray &scattered) const
    {
        attenuation = vec3(1.0, 1.0, 1.0);
        vec3 _reflect = reflect(r.direction(), rec.normal);
        vec3 _refract;
        float ni_over_nt;
        vec3 normal;
        float prob;
        float cosine;

        if (dot(r.direction(), rec.normal) > 0) {
            // ray go into object
            ni_over_nt = ref_idx;
            normal = -rec.normal;
            cosine = ref_idx * dot(r.direction(), rec.normal)
                        / r.direction().length();
        } else {
            // ray go outside of the object
            ni_over_nt = 1 / ref_idx;
            normal = rec.normal;
            cosine = - dot(r.direction(), rec.normal)
                        / r.direction().length();
        }

        if (refract(r.direction(), normal, ni_over_nt, _refract)) {
            prob = schlick(cosine, ref_idx);
        } else {
            prob = 1;
        }

        if (random_double() < prob) {
            scattered = ray(rec.p, _reflect); 
        } else {
            scattered = ray(rec.p, _refract);
        }

        return true;
    }
};