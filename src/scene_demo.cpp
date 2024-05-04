#include "scene_demo.h"
#include "mesh.h"
#include "shader.h"
#include "shadow.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace std;

SceneDemo::SceneDemo()
    : Application(1280, 720, "Scene Demo"),
      camera_(),
      controller_(camera_, 100.0f, 0.03f),
      wireframe_(false),
      animating_(true),
      pointer_locked_(true),
      shader_type_(ShaderType::kPhong),
      blinn_(true),
      shadows_(true) {
    // Capture cursor
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

SceneDemo::~SceneDemo() {
}

int SceneDemo::init() {
    // Turn on vsync
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    if (!load_meshes()) {
        return 1;
    }
    if (!load_shaders()) {
        return 1;
    }
    init_shadow_map();
    init_scene();

    return 0;
}

#define TRY(statement) \
    if (!(statement)) { \
        cerr << __FILE__ << ":" << __LINE__ << ": " << #statement << " failed" << endl; \
        return false; \
    }

bool SceneDemo::load_meshes() {
    mesh_ = make_unique<BasicMesh>();
    TRY(mesh_->load("test.obj"));
    mesh_->setup();

    cube_mesh_ = make_unique<BasicMesh>();
    TRY(cube_mesh_->load("cube.obj"));
    cube_mesh_->setup();

    plane_mesh_ = make_unique<BasicMesh>();
    TRY(plane_mesh_->load("plane.obj"));
    plane_mesh_->setup();

    return true;
}

bool SceneDemo::load_shaders() {
    // Load shader programs
    phong_shader_ = make_unique<ShaderProgram>();
    TRY(phong_shader_->build_from_vf("shader/phong"));

    gouraud_shader_ = make_unique<ShaderProgram>();
    TRY(gouraud_shader_->build_from_vf("shader/gouraud"));

    light_cube_shader_ = make_unique<ShaderProgram>();
    TRY(light_cube_shader_->build_from_vf("shader/light_cube"));

    point_shadow_shader_ = make_unique<ShaderProgram>();
    TRY(point_shadow_shader_->build_from_vgf("shader/point_shadow"));

    return true;
}

#undef TRY

void SceneDemo::init_shadow_map() {
    shadow_map_ = make_unique<PointShadowMap>(2048, 2048, 1.0f, 1024.0f);
}

void SceneDemo::init_scene() {
    camera_.set_position(glm::vec3(250.0f, 250.0f, 250.0f));
    camera_.look_at(glm::vec3(0.0f, 50.0f, 0.0f));

    light_pos_ = glm::vec3(200.0f, 150.0f, 0.0f);
    light_color_ = glm::vec3(1.0f, 1.0f, 1.0f);

    angle_ = 0.0f;
}

int SceneDemo::render() {
    if (animating_) {
        angle_ += 45.0f * delta_time_;
    }

    // 1. Render shadow map
    shadow_map_->bind();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    point_shadow_shader_->use();

    point_shadow_shader_->set_vec3("lightPos", light_pos_);
    point_shadow_shader_->set_float("far", shadow_map_->far());
    for (int i = 0; i < 6; i++) {
        string name = "shadowMatrices[" + to_string(i) + "]";
        point_shadow_shader_->set_mat4(name.c_str(), shadow_map_->shadow_matrix(i));
    }

    render_pass(*point_shadow_shader_, true);

    // 2. Render scene
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width_, height_);
    glClearColor(0.05f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (wireframe_) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    float fov = glm::radians(45.0f);
    float aspect = (float)width_ / height_;
    glm::mat4 projection = glm::perspective(fov, aspect, 0.1f, 1000.0f);

    glm::mat4 view = camera_.view_matrix();

    const ShaderProgram &shader = shader_type_ == ShaderType::kPhong ? *phong_shader_ : *gouraud_shader_;

    shader.use();

    shader.set_mat4("projection", projection);
    shader.set_mat4("view", view);
    shader.set_vec3("viewPos", camera_.position());

    shader.set_vec3("lightPos", light_pos_);
    shader.set_vec3("light.ambient", 0.05f * light_color_);
    shader.set_vec3("light.diffuse", 0.85f * light_color_);
    shader.set_vec3("light.specular", light_color_);
    shader.set_bool("light.blinn", blinn_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_map_->depth_cubemap());
    shader.set_bool("shadows", shadows_);
    shader.set_int("depthCubemap", 0);
    shader.set_float("far", shadow_map_->far());

    render_pass(shader, false);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, light_pos_);
    model = glm::scale(model, glm::vec3(5.0f));

    light_cube_shader_->use();

    light_cube_shader_->set_mat4("projection", projection);
    light_cube_shader_->set_mat4("view", view);
    light_cube_shader_->set_mat4("model", model);
    light_cube_shader_->set_vec3("objectColor", light_color_);

    cube_mesh_->draw();

    return 0;
}

void SceneDemo::render_pass(const ShaderProgram &shader, bool shadow_pass) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(angle_), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, -glm::vec3(300.0f, 50.0f, 0.0f));

    shader.set_mat4("model", model);

    if (!shadow_pass) {
        shader.set_mat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

        glm::vec3 object_color(0.90f, 0.50f, 0.35f);
        shader.set_vec3("material.ambient", object_color);
        shader.set_vec3("material.diffuse", object_color);
        shader.set_vec3("material.specular", glm::vec3(0.5f));
        shader.set_float("material.shininess", 16.0f);
    }

    mesh_->draw();

    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(600.0f, 1.0f, 600.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

    shader.set_mat4("model", model);

    if (!shadow_pass) {
        shader.set_mat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

        glm::vec3 plane_color(0.80f, 0.82f, 0.85f);
        shader.set_vec3("material.ambient", plane_color);
        shader.set_vec3("material.diffuse", 0.5f * plane_color);
        shader.set_vec3("material.specular", glm::vec3(0.5f));
        shader.set_float("material.shininess", 16.0f);
    }

    plane_mesh_->draw();
}

void SceneDemo::process_input() {
    using CameraMovement = FirstPersonController::Movement;
    static const pair<int, CameraMovement> movement_map[] = {
        {GLFW_KEY_W, CameraMovement::kForward},
        {GLFW_KEY_S, CameraMovement::kBackward},
        {GLFW_KEY_A, CameraMovement::kLeft},
        {GLFW_KEY_D, CameraMovement::kRight},
        {GLFW_KEY_SPACE, CameraMovement::kUp},
        {GLFW_KEY_LEFT_SHIFT, CameraMovement::kDown},
    };

    Application::process_input();

    for (const auto &[key, movement] : movement_map) {
        if (glfwGetKey(window_, key) == GLFW_PRESS) {
            controller_.handle_keyboard(movement, delta_time_);
        }
    }
}

void SceneDemo::cursor_pos_callback(double xpos, double ypos) {
    if (!pointer_locked_) {
        return;
    }

    Application::cursor_pos_callback(xpos, ypos);

    auto [delta_x, delta_y] = delta_mouse_pos_;
    controller_.handle_mouse(delta_x, delta_y);
}

void SceneDemo::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        pointer_locked_ = true;
        mouse_pos_.reset();
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void SceneDemo::key_callback(int key, int scancode, int action, int mods) {
    Application::key_callback(key, scancode, action, mods);

    // Escape - unlock cursor / exit
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        if (pointer_locked_) {
            pointer_locked_ = false;
            glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetWindowShouldClose(window_, GLFW_TRUE);
        }
    }

    // Tab - toggle wireframe mode
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        wireframe_ = !wireframe_;
    }

    // 1, 2 - switch shader
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        shader_type_ = ShaderType::kPhong;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        shader_type_ = ShaderType::kGouraud;
    }

    // 3, 4 - switch lighting model
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        blinn_ = true;
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
        blinn_ = false;
    }

    // 5 - toggle shadows
    if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
        shadows_ = !shadows_;
    }

    // P - pause animation
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        animating_ = !animating_;
    }
}
