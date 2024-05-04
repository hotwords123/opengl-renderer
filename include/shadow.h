#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class PointShadowMap {
public:
    PointShadowMap(int width, int height, float near, float far);
    ~PointShadowMap();

    int width() const { return width_; }
    int height() const { return height_; }

    float near() const { return near_; }
    float far() const { return far_; }

    GLuint fbo() const { return fbo_; }
    GLuint depth_cubemap() const { return depth_cubemap_; }

    const glm::mat4 &shadow_matrix(int face) const { return shadow_matrices_[face]; }

    void bind() const;

private:
    int width_;
    int height_;
    float near_;
    float far_;
    GLuint fbo_;
    GLuint depth_cubemap_;
    glm::mat4 shadow_matrices_[6];
};
