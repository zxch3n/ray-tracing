#pragma once
#include "texture.h"
#include "rand.h"

float trilinear_interp(vec3 c[8], float u, float v, float w) {
    float avg = 0;
    float uu = (3 - 2*u) * u * u;
    float vv = (3 - 2*v) * v * v;
    float ww = (3 - 2*w) * w * w;
    for (int i = 0; i < 8; i++) {
        int di = (i & 4) >> 2, dj = (i & 2) >> 1, dk = i & 1;
        vec3 weight_v(u-di, v-dj, w-dk);
        avg += (uu*di + (1-uu)*(1-di)) *
               (vv*dj + (1-vv)*(1-dj)) *
               (ww*dk + (1-ww)*(1-dk)) * dot(c[i], weight_v);
    }

    return avg;
}

class perlin
{
public:
    float noise(const vec3 &p) const
    {
        float u = p.x() - floor(p.x());
        float v = p.y() - floor(p.y());
        float w = p.z() - floor(p.z());
        int i = floor(p.x());
        int j = floor(p.y());
        int k = floor(p.z());
        vec3 c[8];
        for (int index = 0; index < 8; index ++) {
            int di = ((index & 4) >> 2), dj = ((index & 2) >> 1), dk = (index & 1);
            c[index] = ranfloat[perm_x[(i + di) & 255] ^
                                perm_y[(j + dj) & 255] ^
                                perm_z[(k + dk) & 255]];
        }

        return trilinear_interp(c, u, v, w);
    }

    float turb(const vec3 &p, int depth = 7) const
    {
        float accum = 0;
        vec3 temp_p = p;
        float weight = 1.0;
        for (int i = 0; i < depth; i++)
        {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }
        return fabs(accum);
    }

    static vec3 *ranfloat;
    static int *perm_x;
    static int *perm_y;
    static int *perm_z;
};

static vec3* perlin_generate()
{
    vec3 *p = new vec3[256];
    for (int i = 0; i < 256; ++i) { 
        float a = random_double();
        float b = random_double();
        float c = random_double();
        p[i] = unit_vector(vec3(a, b, c) * 2 - vec3(1, 1, 1));
    }

    return p;
}

void permute(int *p, int n)
{
    for (int i = n - 1; i > 0; i--)
    {
        int target = int(random_double() * (i + 1));
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
    return;
}

static int *perlin_generate_perm()
{
    int *p = new int[256];
    for (int i = 0; i < 256; i++)
        p[i] = i;
    permute(p, 256);
    return p;
}

vec3 *perlin::ranfloat = perlin_generate();
int *perlin::perm_x = perlin_generate_perm();
int *perlin::perm_y = perlin_generate_perm();
int *perlin::perm_z = perlin_generate_perm();


class noise_texture : public texture
{
public:
    float scale;
    perlin _noise;
    noise_texture(): scale(1.0) {}
    noise_texture(float scale): scale(scale) {}
    virtual vec3 value(float u, float v, const vec3 &p)
    {
        return vec3(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * _noise.turb(p)));
    }
};