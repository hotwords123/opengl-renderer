#include "control.h"
#include "camera.h"

#include <glm/glm.hpp>

FirstPersonController::FirstPersonController(Camera& camera, float movement_speed, float mouse_sensitivity)
    : camera_(camera),
      movement_speed_(movement_speed),
      mouse_sensitivity_(mouse_sensitivity) {
}

void FirstPersonController::handle_keyboard(Movement movement, float delta_time) {
    glm::vec3 offset;

    switch (movement) {
        case Movement::kForward:
            offset = glm::cross(camera_.world_up(), camera_.right());
            break;
        case Movement::kBackward:
            offset = -glm::cross(camera_.world_up(), camera_.right());
            break;
        case Movement::kLeft:
            offset = -camera_.right();
            break;
        case Movement::kRight:
            offset = camera_.right();
            break;
        case Movement::kUp:
            offset = camera_.world_up();
            break;
        case Movement::kDown:
            offset = -camera_.world_up();
            break;
    }

    camera_.move(offset * movement_speed_ * delta_time);
}

void FirstPersonController::handle_mouse(float x_offset, float y_offset) {
    float pitch = camera_.pitch() - y_offset * mouse_sensitivity_;
    float yaw = camera_.yaw() + x_offset * mouse_sensitivity_;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    } else if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    yaw = fmod(yaw, 360.0f);

    camera_.set_pitch(pitch);
    camera_.set_yaw(yaw);
    camera_.update();
}
