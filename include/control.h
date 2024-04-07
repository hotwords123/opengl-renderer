#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Camera;

enum class CameraMovement {
    kForward,
    kBackward,
    kLeft,
    kRight,
    kUp,
    kDown,
};

class CameraController {
public:
    using enum CameraMovement;

    CameraController(Camera& camera);

    void handle_keyboard(CameraMovement movement, float delta_time);
    void handle_mouse(float x_offset, float y_offset);

private:
    Camera& camera_;

    float movement_speed_;
    float mouse_sensitivity_;
};
