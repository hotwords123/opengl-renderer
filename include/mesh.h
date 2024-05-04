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

    BasicMesh(const BasicMesh &) = delete;
    BasicMesh &operator=(const BasicMesh &) = delete;

    glm::vec3 centroid() const { return centroid_; }
    glm::vec3 min() const { return min_; }
    glm::vec3 max() const { return max_; }

    void cleanup();

    bool load(const char *filename);
    void setup();
    void draw() const;

private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    glm::vec3 centroid_;
    glm::vec3 min_;
    glm::vec3 max_;

    GLuint vbo_, vao_, ebo_;
};
