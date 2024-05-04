#include "mesh.h"

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace std;

using OpenMesh_TriMesh = OpenMesh::TriMesh_ArrayKernelT<>;

bool BasicMesh::load(const char *filename) {
    OpenMesh_TriMesh mesh;

    mesh.request_vertex_normals();
    if (!mesh.has_vertex_normals()) {
        cerr << "ERROR::MESH::VERTEX_NORMALS_NOT_SUPPORTED" << endl;
        return false;
    }

    OpenMesh::IO::Options opt;
    if (!OpenMesh::IO::read_mesh(mesh, filename, opt)) {
        cerr << "ERROR::MESH::FILE_NOT_FOUND\nFILE: " << filename << endl;
        return false;
    }

    if (!opt.check(OpenMesh::IO::Options::VertexNormal)) {
        mesh.request_face_normals();
        mesh.update_normals();
        mesh.release_face_normals();
    }

    vertices_.resize(mesh.n_vertices());
    indices_.clear();
    indices_.reserve(3 * mesh.n_faces());

    auto to_vec3 = [](const auto &rhs) {
        return glm::vec3(rhs[0], rhs[1], rhs[2]);
    };

    auto mesh_point = [&](OpenMesh_TriMesh::VertexHandle vh) {
        return mesh.point(vh);
    };
    centroid_ = to_vec3(mesh.vertices().avg(mesh_point));
    min_ = to_vec3(mesh.vertices().min(mesh_point));
    max_ = to_vec3(mesh.vertices().max(mesh_point));

    for (const auto &v : mesh.vertices()) {
        auto &vertex = vertices_[v.idx()];
        vertex.position = to_vec3(mesh.point(v));
        vertex.normal = to_vec3(mesh.normal(v));
    }

    for (const auto &f : mesh.faces()) {
        for (const auto &v : mesh.fv_range(f)) {
            indices_.push_back(v.idx());
        }
    }

    mesh.release_vertex_normals();
    return true;
}

void BasicMesh::setup() {
    cleanup();

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);
}

void BasicMesh::draw() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void *)0);
    glBindVertexArray(0);
}

void BasicMesh::cleanup() {
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &ebo_);
    vbo_ = vao_ = ebo_ = 0;
}

BasicMesh::~BasicMesh() {
    cleanup();
}
