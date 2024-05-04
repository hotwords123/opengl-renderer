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

ThirdPersonController::ThirdPersonController(Camera& camera, float rotation_speed, float zoom_base)
    : camera_(camera),
      movement_speed_(),
      rotation_speed_(rotation_speed),
      zoom_base_(zoom_base),
      target_(),
      distance_(),
      yaw_(),
      pitch_() {
}

void ThirdPersonController::set_view(float fov, float height) {
    movement_speed_ = 2 * tan(glm::radians(fov) / 2) / height;
}

void ThirdPersonController::rotate(float x_offset, float y_offset) {
    pitch_ -= y_offset * rotation_speed_;
    yaw_ += x_offset * rotation_speed_;

    if (pitch_ > 89.0f) {
        pitch_ = 89.0f;
    } else if (pitch_ < -89.0f) {
        pitch_ = -89.0f;
    }

    yaw_ = fmod(yaw_, 360.0f);

    update_camera();
}

void ThirdPersonController::translate(float x_offset, float y_offset) {
    glm::vec3 offset = x_offset * camera_.right() - y_offset * camera_.up();
    target_ -= movement_speed_ * distance_ * offset;
    update_camera();
}

void ThirdPersonController::zoom(float offset) {
    distance_ *= pow(zoom_base_, offset);
    update_camera();
}

void ThirdPersonController::update_camera() const {
    glm::vec3 front(
        cos(glm::radians(yaw_)) * cos(glm::radians(pitch_)),
        sin(glm::radians(pitch_)),
        sin(glm::radians(yaw_)) * cos(glm::radians(pitch_))
    );
    camera_.set_position(target_ - distance_ * front);
    camera_.look_at(target_);
}
