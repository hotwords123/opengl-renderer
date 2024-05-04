#pragma once

#include <glm/glm.hpp>

class Camera;

class FirstPersonController {
public:
    enum class Movement {
        kForward,
        kBackward,
        kLeft,
        kRight,
        kUp,
        kDown,
    };
    using enum Movement;

    FirstPersonController(Camera& camera, float movement_speed, float mouse_sensitivity);

    void handle_keyboard(Movement movement, float delta_time);
    void handle_mouse(float x_offset, float y_offset);

private:
    Camera& camera_;

    float movement_speed_;
    float mouse_sensitivity_;
};

class ThirdPersonController {
public:
    ThirdPersonController(Camera& camera, float rotation_speed, float zoom_base);

    void set_view(float fov, float height);

    glm::vec3 target() const { return target_; }
    void set_target(glm::vec3 target) { target_ = target; }

    float distance() const { return distance_; }
    void set_distance(float distance) { distance_ = distance; }

    float yaw() const { return yaw_; }
    void set_yaw(float yaw) { yaw_ = yaw; }

    float pitch() const { return pitch_; }
    void set_pitch(float pitch) { pitch_ = pitch; }

    void rotate(float x_offset, float y_offset);
    void translate(float x_offset, float y_offset);
    void zoom(float offset);

    void update_camera() const;

private:
    Camera& camera_;

    float movement_speed_;
    float rotation_speed_;
    float zoom_base_;

    glm::vec3 target_;
    float distance_;
    float yaw_;
    float pitch_;
};
