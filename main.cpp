#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "vec3.h"
#include "math.h"
#include "ray.h"
#include "hittable_list.h"
#include "sphere.h"
#include "rand.h"
#include "camera.h"
#include "material.h"
#include "cuboid.h"
#include "bvh.h"
#include "texture.h"
#include "perlin.h"
#include <float.h>
#include <vector>
#include "./stb/stb_image.h"


bool hit_sphere(const vec3& center, float radius, const ray& r) {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0 * dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - 4*a*c;
    return (discriminant > 0);
}

vec3 color(const ray &r, const hittable *world, int depth,
           vec3 acc_emitted = vec3(0, 0, 0),
           vec3 acc_attenuate = vec3(1, 1, 1))
{
    if (depth > 100)
    {
        return acc_emitted * acc_attenuate;
    }

    if (acc_attenuate[0] + acc_attenuate[1] + acc_attenuate[2] < 1 / 256.0) {
        return acc_emitted * acc_attenuate;
    }

    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuate;
        const vec3 emitted = rec.m->emit(rec.u, rec.v, rec.p);
        if (rec.m->scatter(r, rec, attenuate, scattered)) {
            return color(scattered, world, depth + 1, (emitted + acc_emitted), attenuate * acc_attenuate);
        } else {
            vec3 ans = (emitted + acc_emitted) * acc_attenuate;
            return ans;
        }
    }

    return acc_emitted * acc_attenuate;
}

hittable_list *random_scene() {
    int n = 500;
    std::vector<hittable*> *vlist = new std::vector<hittable*>(n);
    std::vector<hittable*>& list = *vlist;
    checker_texture *checker = new checker_texture(
        new constant_texture(vec3(0.8, 0.2, 0.2)),
        new constant_texture(vec3(0.7, 0.7, 0.8))
    );
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.95, 0.95, 0.95))));
    int i = 1;
    for (int a = -2; a < 2; a++) {
        for (int b = -2; b < 2; b++) {
            float choose_mat = random_double();
            vec3 center(a+0.9*random_double(),0.2,b+0.9*random_double());
            material *m;
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                        m = new lambertian(new constant_texture(vec3(random_double()*random_double(),
                                            random_double()*random_double(),
                                            random_double()*random_double())));
                }
                else if (choose_mat < 0.95) { // metal
                        m = new metal(vec3(0.5*(1 + random_double()),
                                           0.5*(1 + random_double()),
                                           0.5*(1 + random_double())),
                                      0.5*random_double());
                }
                else {  // glass
                    m = new dielectric(1.5);
                }
            }

            if (random_double() < 0.5) {
                list[i++] = new sphere(center, 0.2, m);
            } else {
                list[i++] = new cuboid(center, vec3(0.4, 0.4, 0.4), m);
            }
        }
    }

    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
    material *mat = new lambertian(new image_texture(tex_data, nx, ny));
    list[i++] = new sphere(vec3(-1, 1.5, 0.3), 0.3, mat);
    list[i++] = new sphere(vec3(0.6, 1, 0), 0.2, new metal(vec3(1.0, 1.0, 1.0), 0.0));

    list[i++] = new cuboid(vec3(0, 3, 0), vec3(2, 1e-6, 2), new diffuse_light(new constant_texture(vec3(10, 10, 10))));
    list[i++] = new cuboid(vec3(0, 3.00001, 0), vec3(4, 1e-6, 4), new metal(vec3(1.0, 1.0, 1.0), 0));
    list[i++] = new cuboid(vec3(0, 1.5, -2), vec3(4, 3, 1e-6), new metal(vec3(1.0, 1.0, 1.0), 0));
    list[i++] = new cuboid(vec3(0, 1.5, 4), vec3(4, 3, 1e-6), new metal(vec3(1.0, 1.0, 1.0), 0));
    list[i++] = new cuboid(vec3(2, 1.5, 0), vec3(1e-6, 3, 4), new luminating_mirror(new constant_texture(vec3(0, 0, 0))));
    list[i++] = new cuboid(vec3(2.0001, 1.5, 0), vec3(1e-6, 1, 1), new diffuse_light(new constant_texture(vec3(10.0, 10.0, 10.0))));
    list[i++] = new cuboid(vec3(-2, 1.5, 0), vec3(1e-6, 3, 4), new luminating_mirror(new constant_texture(vec3(0.0, 0.0, 0))));
    list[i++] = new cuboid(vec3(-1.9999, 1.5, 0), vec3(1e-6, 1, 1), new diffuse_light(new constant_texture(vec3(10.0, 10.0, 10.0))));
    list.resize(i);

    hittable **ans = new hittable*[1];
    ans[0] = new bvh(list, 0, i);
    return new hittable_list(ans, 1);
}

int main()
{
    int nx = 1200;
    int ny = 1200;
    int n_sample = 2000;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    hittable_list *world = random_scene();
    camera c(vec3(0, 1.9, 3.9), vec3(0, 1, 0), vec3(0, 1, 0), 0.02, 65, float(nx) / float(ny));
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
            col.clamp_(0, 1);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}