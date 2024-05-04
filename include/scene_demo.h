#pragma once

#include "application.h"
#include "camera.h"
#include "control.h"

#include <glm/glm.hpp>

#include <memory>

class BasicMesh;
class ShaderProgram;
class PointShadowMap;

class SceneDemo : public Application {
public:
    enum class ShaderType {
        kPhong,
        kGouraud,
    };
    using enum ShaderType;

    explicit SceneDemo();
    ~SceneDemo();

    void process_input() override;
    void cursor_pos_callback(double xpos, double ypos) override;
    void mouse_button_callback(int button, int action, int mods) override;
    void key_callback(int key, int scancode, int action, int mods) override;

protected:
    int init() override;
    int render() override;

private:
    bool load_meshes();
    bool load_shaders();
    void init_shadow_map();
    void init_scene();
    void render_pass(const ShaderProgram &shader, bool shadow_pass);

private:
    Camera camera_;
    FirstPersonController controller_;

    std::unique_ptr<BasicMesh> mesh_;
    std::unique_ptr<BasicMesh> cube_mesh_;
    std::unique_ptr<BasicMesh> plane_mesh_;

    std::unique_ptr<ShaderProgram> phong_shader_;
    std::unique_ptr<ShaderProgram> gouraud_shader_;
    std::unique_ptr<ShaderProgram> light_cube_shader_;
    std::unique_ptr<ShaderProgram> point_shadow_shader_;

    std::unique_ptr<PointShadowMap> shadow_map_;

    glm::vec3 light_pos_;
    glm::vec3 light_color_;

    float angle_;

    bool wireframe_;
    bool animating_;
    bool pointer_locked_;

    ShaderType shader_type_;
    bool blinn_;
    bool shadows_;
};
