#pragma once

#include "camera.h"
#include "control.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum class ShaderType {
    kPhong,
    kGouraud,
};

class Application {
public:
    using enum ShaderType;

    explicit Application(GLFWwindow *window);
    ~Application();

    int exec();

    void process_input(float delta_time);
    void framebuffer_size_callback(int width, int height);
    void cursor_pos_callback(double xpos, double ypos);
    void mouse_button_callback(int button, int action, int mods);
    void key_callback(int key, int scancode, int action, int mods);

private:
    GLFWwindow* window_;
    int width_;
    int height_;

    Camera camera_;
    CameraController controller_;

    float last_time_;

    bool wireframe_;
    bool animating_;
    bool pointer_locked_;

    ShaderType shader_type_;
    bool blinn_;
    bool shadows_;

    bool first_mouse_;
    double last_x_;
    double last_y_;
};
