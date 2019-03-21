#include <iostream>
#include <fstream>
#include <cfloat>
#include <vector>

#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "bvh.h"
#include "texture.h"
#include "rectangle.h"
#include "box.h"
#include "material.h"
#include "triangle.h"

using namespace std;


vec3 color(const ray &r, hitable *world, pdf *light_pdf, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 emitted = rec.mat_ptr->emitted(r, rec);
        scatter_record srec;
        float pdf_value;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, srec)) {
            if (light_pdf == nullptr) {
                scattered = ray(rec.p, srec.pdf_ptr->generate(rec), r.time());
                pdf_value = srec.pdf_ptr->value(rec, scattered.direction());
            } else {
                mixture_pdf p(light_pdf, srec.pdf_ptr);
                scattered = ray(rec.p, p.generate(rec), r.time());
                pdf_value = p.value(rec, scattered.direction());
            }

            return emitted +
                   srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) *
                   color(scattered, world, light_pdf, depth + 1) / pdf_value;
        } else {
            return emitted;
        }
    } else {
        return vec3(1, 1, 1);
    }
}

hitable *cornell_box() {
    hitable **list = new hitable *[9];
    int i = 0;
    pdf *cosine = new cosine_pdf();
    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)), cosine);
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)), cosine);
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)), cosine);
    material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
//    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
//    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
//    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
//    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
//    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
//    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
//    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
    list[i++] = new triangle(vec3(0, 0, 0), vec3(500, 0, 500), vec3(500, 0, 0), red);

    return new hitable_list(list, i);
}

void load_obj(const string &name, vector<hitable *> &mesh) {
    ifstream fs(name, ios::in);
    int vm, fm;
    fs >> vm >> fm;
    char s;
    float x, y, z;
    vector<vec3> points;
    vector<vec3> faces;
    for (int i = 0; i < vm; ++i) {
        fs >> s >> x >> y >> z;
        vec3 p(x, y, z);
        points.emplace_back(p*100);
    }
    int a, b, c;
    for (int i = 0; i < fm; ++i) {
        fs >> s >> a >> b >> c;
        vec3 t(a - 1, b - 1, c - 1);
        faces.emplace_back(t);
    }
    fs.close();

    pdf *cosine = new cosine_pdf();
    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)), cosine);
    for (int i = 0; i < faces.size(); ++i) {
        a = (int) faces[i][0];
        b = (int) faces[i][1];
        c = (int) faces[i][2];
        hitable *tri = new triangle(points[a], points[b], points[c], red);
        mesh.emplace_back(tri);
    }
}

hitable *scene() {
    vector<hitable *> mesh;
    load_obj("../homer_large.obj", mesh);

    return new bvh(mesh.data(), mesh.size(), 0, FLT_MAX);
}

int main() {
    ofstream outfile("hellograph.ppm");
    int nx = 200, ny = 200, ns = 100;
    vec3 lookfrom(0, 0, 400);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;

    outfile << "P3\n" << nx << " " << ny << "\n255\n";

    camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);
    hitable *world = scene();
//    hitable *light_shape = new xz_rect(213, 343, 227, 332, 554, nullptr);
//    pdf *light_pdf = new hitable_pdf(light_shape);

    for (int j = ny - 1; j >= 0; --j) {
        cout << j << endl;
        for (int i = 0; i < nx; ++i) {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; ++s) {
                float u = float(i + drand48()) / nx;
                float v = float(j + drand48()) / ny;
                ray r = cam.get_ray(u, v);
                col += color(r, world, nullptr, 0);
            }

            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));//gamma corrected
            int ir = int(ffmin(255.99 * col[0], 255));
            int ig = int(ffmin(255.99 * col[1], 255));
            int ib = int(ffmin(255.99 * col[2], 255));
            outfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    outfile.close();

    return 0;
}