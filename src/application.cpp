#include "application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace std;

static Application *get_app(GLFWwindow* window) {
    return static_cast<Application *>(glfwGetWindowUserPointer(window));
}

static void global_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    get_app(window)->framebuffer_size_callback(width, height);
}

static void global_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    get_app(window)->cursor_pos_callback(xpos, ypos);
}

static void global_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    get_app(window)->mouse_button_callback(button, action, mods);
}

static void global_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    get_app(window)->key_callback(key, scancode, action, mods);
}

Application::Application(int width, int height, const char *title)
    : window_(),
      width_(width),
      height_(height),
      time_(),
      delta_time_(),
      mouse_pos_(),
      delta_mouse_pos_() {
    // Create window
    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window_ == nullptr) {
        cerr << "Failed to create GLFW window" << endl;
        return;
    }

    // Initialize window
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, global_framebuffer_size_callback);
    glfwSetCursorPosCallback(window_, global_cursor_pos_callback);
    glfwSetMouseButtonCallback(window_, global_mouse_button_callback);
    glfwSetKeyCallback(window_, global_key_callback);
}

Application::~Application() {
    if (window_ != nullptr) {
        glfwDestroyWindow(window_);
    }
}

int Application::exec() {
    if (window_ == nullptr) {
        return 1;
    }

    glfwMakeContextCurrent(window_);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        return 1;
    }

    if (int ret = init(); ret != 0) {
        return ret;
    }

    // Main loop
    while (!glfwWindowShouldClose(window_)) {
        float last_time = time_;
        time_ = (float)glfwGetTime();
        delta_time_ = time_ - last_time;

        process_input();

        if (int ret = render(); ret != 0) {
            return ret;
        }

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    return 0;
}

void Application::process_input() {
}

void Application::framebuffer_size_callback(int width, int height) {
    width_ = width;
    height_ = height;
}

void Application::cursor_pos_callback(double xpos, double ypos) {
    float x = (float)xpos;
    float y = (float)ypos;

    if (!mouse_pos_) {
        mouse_pos_ = make_pair(x, y);
        return;
    }

    auto [last_x, last_y] = *mouse_pos_;
    mouse_pos_ = make_pair(x, y);
    delta_mouse_pos_ = make_pair(x - last_x, y - last_y);
}

void Application::mouse_button_callback(int button, int action, int mods) {
    (void)button;
    (void)action;
    (void)mods;
}

void Application::key_callback(int key, int scancode, int action, int mods) {
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
}
