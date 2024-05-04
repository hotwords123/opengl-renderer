#pragma once

#include "application.h"
#include "camera.h"
#include "control.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <string>

class BasicMesh;
class CircleMesh;
class ShaderProgram;

class SimpleRenderer : public Application {
public:
    enum class ShaderType {
        kPhong,
        kGouraud,
    };
    using enum ShaderType;

    explicit SimpleRenderer(std::vector<std::string> obj_files);
    ~SimpleRenderer();

    void process_input() override;
    void framebuffer_size_callback(int width, int height) override;
    void cursor_pos_callback(double xpos, double ypos) override;
    void mouse_button_callback(int button, int action, int mods) override;
    void key_callback(int key, int scancode, int action, int mods) override;
    void scroll_callback(double xoffset, double yoffset) override;

protected:
    int init() override;
    int render() override;

private:
    bool load_meshes();
    bool load_shaders();
    void init_scene();

private:
    Camera camera_;
    ThirdPersonController controller_;

    std::vector<std::string> obj_files_;
    std::vector<std::unique_ptr<BasicMesh>> meshes_;
    std::unique_ptr<CircleMesh> circle_mesh_;

    std::unique_ptr<ShaderProgram> phong_shader_;
    std::unique_ptr<ShaderProgram> gouraud_shader_;
    std::unique_ptr<ShaderProgram> circle_shader_;

    glm::vec3 light_pos_;
    glm::vec3 light_color_;

    bool wireframe_;
    bool trackball_;

    ShaderType shader_type_;
    bool blinn_;
};
