#include "control.h"
#include "camera.h"

#include <glm/glm.hpp>

CameraController::CameraController(Camera& camera)
    : camera_(camera),
      movement_speed_(100.0f),
      mouse_sensitivity_(0.03f) {
}

void CameraController::handle_keyboard(CameraMovement movement, float delta_time) {
    glm::vec3 offset;

    switch (movement) {
        case CameraMovement::kForward:
            offset = glm::cross(camera_.world_up(), camera_.right());
            break;
        case CameraMovement::kBackward:
            offset = -glm::cross(camera_.world_up(), camera_.right());
            break;
        case CameraMovement::kLeft:
            offset = -camera_.right();
            break;
        case CameraMovement::kRight:
            offset = camera_.right();
            break;
        case CameraMovement::kUp:
            offset = camera_.world_up();
            break;
        case CameraMovement::kDown:
            offset = -camera_.world_up();
            break;
    }

    camera_.move(offset * movement_speed_ * delta_time);
}

void CameraController::handle_mouse(float x_offset, float y_offset) {
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
