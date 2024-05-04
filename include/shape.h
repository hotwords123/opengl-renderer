#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class CircleMesh {
public:
    struct Vertex {
        glm::vec3 position;
    };

    CircleMesh(int segments);
    ~CircleMesh();

    CircleMesh(const CircleMesh &) = delete;
    CircleMesh &operator=(const CircleMesh &) = delete;

    void draw() const;

private:
    std::vector<Vertex> vertices_;

    GLuint vbo_, vao_;
};
