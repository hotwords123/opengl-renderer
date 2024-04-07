#pragma once

#include "camera.h"
#include "control.h"
#include "mesh.h"
#include "shader.h"

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
    void mouse_callback(double xpos, double ypos);

private:
    GLFWwindow* window_;
    int width_;
    int height_;

    Camera camera_;
    CameraController controller_;

    float last_time_;
    bool wireframe_;
    ShaderType shader_type_;
    bool blinn_;
    bool first_mouse_;
    double last_x_;
    double last_y_;
};
