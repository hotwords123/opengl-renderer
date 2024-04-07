#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

class BasicMesh {
public:
    BasicMesh() : vbo_(0), vao_(0), ebo_(0) {}
    ~BasicMesh();

    void cleanup();

    bool load(const char *filename);
    void setup();
    void draw() const;

private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;

    GLuint vbo_, vao_, ebo_;
};
