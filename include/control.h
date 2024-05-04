#pragma once

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
