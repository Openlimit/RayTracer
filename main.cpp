#include <iostream>
#include <fstream>
#include <cfloat>
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "lambertian.h"
#include "metal.h"
#include "dielectric.h"
#include "moving_sphere.h"
#include "bvh.h"
#include "texture.h"

using namespace std;


vec3 color(const ray &r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth + 1);
        } else {
            return vec3(0, 0, 0);
        }
    } else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5f * (unit_direction.y() + 1.0f);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable *[n + 1];
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                                           new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    int i = 1;
    for (int a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; ++b) {
            float choose_mat = drand48();
            vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {//diffuse
                    list[i++] = new sphere(center, 0.2, new lambertian(
                            new constant_texture(
                                    vec3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48()))));
//                    list[i++] = new moving_sphere(center, center + vec3(0, 0.5 * drand48(), 0), 0.0, 1.0, 0.2,
//                                                  new lambertian(vec3(drand48() * drand48(), drand48() * drand48(),
//                                                                      drand48() * drand48())));
                } else if (choose_mat < 0.95) {//metal
                    list[i++] = new sphere(center, 0.2, new metal(
                            vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.5 * (1 + drand48())),
                            0.5 * drand48()));
                } else {//glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
//    return new hitable_list(list, i);
    return new bvh(list, i, 0, 0);
}

hitable *two_sphere() {
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                                           new constant_texture(vec3(0.9, 0.9, 0.9)));
    int n = 50;
    hitable **list = new hitable *[n + 1];
    list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian(checker));
    list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian(checker));

    return new hitable_list(list, 2);
}

int main() {
    ofstream outfile("hellograph.ppm");
    int nx = 800, ny = 400, ns = 100;
    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = 10;
    float aperture = 0.0;

    outfile << "P3\n" << nx << " " << ny << "\n255\n";

    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);
//    hitable *world = random_scene();
    hitable *world = two_sphere();
    for (int j = ny - 1; j >= 0; --j) {
        cout << j << endl;
        for (int i = 0; i < nx; ++i) {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; ++s) {
                float u = float(i + drand48()) / nx;
                float v = float(j + drand48()) / ny;
                ray r = cam.get_ray(u, v);
                col += color(r, world, 0);
            }

            col /= ns;
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            outfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    outfile.close();

    return 0;
}