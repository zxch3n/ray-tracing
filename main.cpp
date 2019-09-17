#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "hittable_list.h"
#include "sphere.h"
#include "rand.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include <float.h>
#include <vector>


bool hit_sphere(const vec3& center, float radius, const ray& r) {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0 * dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - 4*a*c;
    return (discriminant > 0);
}

vec3 color(const ray& r, const hittable* world, int depth) {
    if (depth > 50) {
        return vec3(0, 0, 0);
    }

    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuate;
        if (rec.m->scatter(r, rec, attenuate, scattered)) {
            return attenuate * color(scattered, world, depth + 1);
        } else {
            return vec3(0, 0, 0);
        }
    }

    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}

hittable_list *random_scene() {
    int n = 500;
    std::vector<hittable*> *vlist = new std::vector<hittable*>(n);
    std::vector<hittable*>& list = *vlist;
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = random_double();
            vec3 center(a+0.9*random_double(),0.2,b+0.9*random_double());
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new sphere(center, 0.2,
                        new lambertian(vec3(random_double()*random_double(),
                                            random_double()*random_double(),
                                            random_double()*random_double())
                        )
                    );
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2,
                            new metal(vec3(0.5*(1 + random_double()),
                                           0.5*(1 + random_double()),
                                           0.5*(1 + random_double())),
                                      0.5*random_double()));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
    list.resize(i);

    hittable **ans = new hittable*[1];
    ans[0] = new bvh(list, 0, i);
    return new hittable_list(ans, 1);
}

int main()
{
    int nx = 600;
    int ny = 600;
    int n_sample = 1;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    hittable_list *world = random_scene();
    camera c(vec3(-10, 1.5, 4), vec3(0, 1, 0), vec3(0, 1, 0), 0.001, 45, float(nx) / float(ny));
    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 color_sum(0, 0, 0);
            for (int k = 0; k < n_sample; k++){
                float u = (float(i) + random_double()) / float(nx);
                float v = (float(j) + random_double()) / float(ny);
                color_sum += color(c.get_ray(u, v), world, 0);
            }

            vec3 col = color_sum / n_sample;
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}