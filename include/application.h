#pragma once

#include <memory>
#include <optional>
#include <utility>

struct GLFWwindow;

class Application {
public:
    explicit Application(int width, int height, const char *title);
    virtual ~Application();

    int exec();

    virtual void process_input();
    virtual void framebuffer_size_callback(int width, int height);
    virtual void cursor_pos_callback(double xpos, double ypos);
    virtual void mouse_button_callback(int button, int action, int mods);
    virtual void key_callback(int key, int scancode, int action, int mods);
    virtual void scroll_callback(double xoffset, double yoffset);

protected:
    virtual int init() = 0;
    virtual int render() = 0;

protected:
    GLFWwindow* window_;
    int width_;
    int height_;

    float time_;
    float delta_time_;

    std::optional<std::pair<float, float>> mouse_pos_;
    std::pair<float, float> delta_mouse_pos_;
};
