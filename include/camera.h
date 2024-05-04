#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    Camera(glm::vec3 position, glm::vec3 world_up, float yaw, float pitch);

    glm::vec3 position() const { return position_; }
    void set_position(glm::vec3 position) { position_ = position; }

    glm::vec3 world_up() const { return world_up_; }
    void set_world_up(glm::vec3 world_up) { world_up_ = world_up; }

    float yaw() const { return yaw_; }
    void set_yaw(float yaw) { yaw_ = yaw; }

    float pitch() const { return pitch_; }
    void set_pitch(float pitch) { pitch_ = pitch; }

    glm::vec3 front() const { return front_; }
    glm::vec3 right() const { return right_; }
    glm::vec3 up() const { return up_; }

    glm::mat4 view_matrix() const { return view_matrix_; }

    void update();

    void move(glm::vec3 offset);
    void look_at(glm::vec3 target);

private:
    glm::vec3 position_;
    glm::vec3 world_up_;

    float yaw_;
    float pitch_;

    glm::vec3 front_;
    glm::vec3 right_;
    glm::vec3 up_;

    glm::mat4 view_matrix_;

    void update_view_matrix();
};
