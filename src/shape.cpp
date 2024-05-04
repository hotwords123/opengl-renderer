#include "shape.h"

CircleMesh::CircleMesh(int segments)
    : vertices_(segments), vbo_(), vao_() {
    for (int i = 0; i < segments; ++i) {
        float theta = glm::radians(360.0f * i / segments);
        vertices_[i].position = glm::vec3(cos(theta), 0.0f, sin(theta));
    }

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

CircleMesh::~CircleMesh() {
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
}

void CircleMesh::draw() const {
    glBindVertexArray(vao_);
    glDrawArrays(GL_LINE_LOOP, 0, vertices_.size());
    glBindVertexArray(0);
}
