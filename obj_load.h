//
// Created by 王惠 on 2019/3/22.
//

#ifndef RAYTRACER_OBJ_LOAD_H
#define RAYTRACER_OBJ_LOAD_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <regex>

#include "triangle.h"
#include "material.h"

using namespace std;

void get_face(const string &face_line, vector<vec3i> &face) {
    regex re("(\\d*)/\\d*/(\\d*)");
    smatch sm;
    string s = face_line;
    while (regex_search(s, sm, re)) {
        int p_id = stoi(sm.format("$1"));
        int vn_id = stoi(sm.format("$2"));
        face.emplace_back(vec3i(p_id - 1, 0, vn_id - 1));//序号从0开始
        s = sm.suffix().str();
    }
}

void load_mtl(const string &name, map<string, Phong *> &mtl_map) {
    ifstream fs(name, ios::in);

    string flag;
    mtl m;
    string mtl_name;
    bool begin = true;
    while (fs.good()) {
        fs >> flag;
        if (flag == "newmtl") {
            if (!begin) {
                mtl_map[mtl_name] = new Phong(m);
                m = mtl();
            } else
                begin = false;
            fs >> mtl_name;
        } else if (flag == "illum")
            fs >> m.illum;
        else if (flag == "Kd")
            fs >> m.kd[0] >> m.kd[1] >> m.kd[2];
        else if (flag == "Ka")
            fs >> m.ka[0] >> m.ka[1] >> m.ka[2];
        else if (flag == "Tf")
            fs >> m.tf[0] >> m.tf[1] >> m.tf[2];
        else if (flag == "Ni")
            fs >> m.ni;
        else if (flag == "Ks")
            fs >> m.ks[0] >> m.ks[1] >> m.ks[2];
        else if (flag == "Ns")
            fs >> m.ns;
        else
            cerr << "load_mtl fail: " << flag << endl;
    }
    if (!begin)
        mtl_map[mtl_name] = new Phong(m);
}

void load_obj(const string &dir_path, const string &name, vector<hitable *> &mesh) {
    ifstream fs(dir_path + name, std::ios::in);
    map<string, Phong *> mtl_map;
    vector<vec3> points;
    vector<vec3> vns;
    vector<vec3i> face;

    string flag;
    string cur_mtl;
    while (fs.good()) {
        fs >> flag;
        if (flag[0] == '#' || flag == "g" || flag == "s") {
            string s;
            getline(fs, s);
        } else if (flag == "mtllib") {
            string mtl_name;
            fs >> mtl_name;
            load_mtl(dir_path + mtl_name, mtl_map);
        } else if (flag == "v") {
            vec3 p;
            fs >> p[0] >> p[1] >> p[2];
            points.emplace_back(p);
        } else if (flag == "vn") {
            vec3 vn;
            fs >> vn[0] >> vn[1] >> vn[2];
            vns.emplace_back(vn);
        } else if (flag == "vt") {
            string s;
            getline(fs, s);
        } else if (flag == "usemtl") {
            fs >> cur_mtl;
        } else if (flag == "f") {
            string face_line;
            getline(fs, face_line);
            face.clear();
            get_face(face_line, face);
            if (mtl_map.find(cur_mtl) == mtl_map.end()) {
                cerr << "mtl_map error：" << cur_mtl << endl;
            }

            vec3 ka = mtl_map[cur_mtl]->phong_mtl.ka;
            if (ka[0] > 0 || ka[1] > 0 || ka[2] > 0)
                continue;
//            if (cur_mtl != string("breakfast_room_cup_Material_005"))
//                continue;

            if (face.size() == 3) {
                hitable *tri = new triangle(points[face[0][0]], points[face[1][0]], points[face[2][0]],
                                            vns[face[0][2]], vns[face[1][2]], vns[face[2][2]], mtl_map[cur_mtl]);
                mesh.emplace_back(tri);
            } else if (face.size() == 4) {
                hitable *tri1 = new triangle(points[face[0][0]], points[face[1][0]], points[face[2][0]],
                                             vns[face[0][2]], vns[face[1][2]], vns[face[2][2]], mtl_map[cur_mtl]);
                mesh.emplace_back(tri1);

                hitable *tri2 = new triangle(points[face[0][0]], points[face[2][0]], points[face[3][0]],
                                             vns[face[0][2]], vns[face[2][2]], vns[face[3][2]], mtl_map[cur_mtl]);
                mesh.emplace_back(tri2);
            } else {
                continue;
            }
        } else {
            string s;
            getline(fs, s);
            cerr << "load face error：" << flag << endl;
        }
    }

}

#endif //RAYTRACER_OBJ_LOAD_H
