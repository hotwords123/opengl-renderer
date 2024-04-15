#include "shadow.h"

#include <glm/gtc/matrix_transform.hpp>

static const glm::vec3 kCubemapDirections[6] = {
    {1.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, -1.0f},
};
static const glm::vec3 kCubemapUps[6] = {
    {0.0f, -1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, -1.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
};

PointShadowMap::PointShadowMap(int width, int height, float near, float far)
    : width_(width),
      height_(height),
      near_(near),
      far_(far),
      fbo_(),
      depth_cubemap_() {
    // Create a framebuffer object and a depth cubemap texture
    glGenFramebuffers(1, &fbo_);
    glGenTextures(1, &depth_cubemap_);

    // Initialize the depth cubemap texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap_);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
        );
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Attach the depth cubemap texture to the framebuffer object
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Calculate shadow matrices
    float aspect = (float)width_ / height_;
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), aspect, near_, far_);

    for (int i = 0; i < 6; ++i) {
        shadow_matrices_[i] = proj * glm::lookAt(
            glm::vec3(0.0f), kCubemapDirections[i], kCubemapUps[i]
        );
    }
}

PointShadowMap::~PointShadowMap() {
    glDeleteFramebuffers(1, &fbo_);
    glDeleteTextures(1, &depth_cubemap_);
}

void PointShadowMap::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, width_, height_);
    glClear(GL_DEPTH_BUFFER_BIT);
}
