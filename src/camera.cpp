#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

using namespace std;

Camera::Camera(glm::vec3 position, glm::vec3 world_up, float yaw, float pitch)
    : position_(position), world_up_(world_up), yaw_(yaw), pitch_(pitch) {
    update();
}

void Camera::update_view_matrix() {
    view_matrix_ = glm::lookAt(position_, position_ + front_, up_);
}

void Camera::update() {
    front_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_.y = sin(glm::radians(pitch_));
    front_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::cross(right_, front_);

    update_view_matrix();
}

void Camera::move(glm::vec3 offset) {
    position_ += offset;
    update_view_matrix();
}

void Camera::look_at(glm::vec3 target) {
    front_ = glm::normalize(target - position_);
    pitch_ = glm::degrees(asin(front_.y));
    yaw_ = glm::degrees(atan2(front_.z, front_.x));

    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::cross(right_, front_);

    update_view_matrix();
}
