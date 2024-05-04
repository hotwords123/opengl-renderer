#include "simple_renderer.h"
#include "mesh.h"
#include "shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cmath>

using namespace std;

SimpleRenderer::SimpleRenderer(vector<string> obj_files)
    : Application(1280, 720, "Simple Renderer"),
      camera_(),
      controller_(camera_, 0.125f, 1.1f),
      obj_files_(std::move(obj_files)),
      wireframe_(false),
      shader_type_(ShaderType::kPhong),
      blinn_(true) {
}

SimpleRenderer::~SimpleRenderer() {
}

int SimpleRenderer::init() {
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
    init_scene();

    return 0;
}

#define TRY(statement) \
    if (!(statement)) { \
        cerr << __FILE__ << ":" << __LINE__ << ": " << #statement << " failed" << endl; \
        return false; \
    }

bool SimpleRenderer::load_meshes() {
    if (obj_files_.empty()) {
        cerr << "No OBJ files specified" << endl;
        return false;
    }

    meshes_.reserve(obj_files_.size());
    for (const auto &obj_file : obj_files_) {
        auto mesh = make_unique<BasicMesh>();
        TRY(mesh->load(obj_file.c_str()));
        mesh->setup();
        meshes_.push_back(std::move(mesh));
    }
    return true;
}

bool SimpleRenderer::load_shaders() {
    phong_shader_ = make_unique<ShaderProgram>();
    TRY(phong_shader_->build_from_vf("shader/phong"));

    gouraud_shader_ = make_unique<ShaderProgram>();
    TRY(gouraud_shader_->build_from_vf("shader/gouraud"));

    return true;
}

#undef TRY

void SimpleRenderer::init_scene() {
    // Compute scene bounding box
    glm::vec3 min(numeric_limits<float>::max());
    glm::vec3 max(numeric_limits<float>::lowest());
    for (const auto &mesh : meshes_) {
        min = glm::min(min, mesh->min());
        max = glm::max(max, mesh->max());
    }

    // Initialize camera
    controller_.set_view(45.0f, height_);
    controller_.set_target(0.5f * (min + max));
    controller_.set_distance(1.5f * glm::length(max - min));
    controller_.set_yaw(0.0f);
    controller_.set_pitch(0.0f);
    controller_.update_camera();

    // Initialize light
    light_pos_ = camera_.position();
    light_color_ = glm::vec3(1.0f);
}

int SimpleRenderer::render() {
    glViewport(0, 0, width_, height_);
    glClearColor(0.05f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (wireframe_) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    light_pos_ = camera_.position();

    float fov = glm::radians(45.0f);
    float aspect = (float)width_ / height_;
    glm::mat4 projection = glm::perspective(fov, aspect, 0.1f, 10000.0f);

    glm::mat4 view = camera_.view_matrix();

    const ShaderProgram &shader = shader_type_ == ShaderType::kPhong ? *phong_shader_ : *gouraud_shader_;

    shader.use();

    shader.set_mat4("projection", projection);
    shader.set_mat4("view", view);
    shader.set_vec3("viewPos", camera_.position());

    shader.set_vec3("lightPos", light_pos_);
    shader.set_vec3("light.ambient", 0.05f * light_color_);
    shader.set_vec3("light.diffuse", 0.75f * light_color_);
    shader.set_vec3("light.specular", 0.4f * light_color_);
    shader.set_bool("light.blinn", blinn_);
    shader.set_bool("shadows", false);

    for (auto &mesh : meshes_) {
        glm::mat4 model = glm::mat4(1.0f);
        shader.set_mat4("model", model);
        shader.set_mat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

        glm::vec3 object_color(0.75f, 0.75f, 0.75f);
        shader.set_vec3("material.ambient", object_color);
        shader.set_vec3("material.diffuse", object_color);
        shader.set_vec3("material.specular", glm::vec3(1.0f));
        shader.set_float("material.shininess", 16.0f);

        mesh->draw();
    }

    return 0;
}

void SimpleRenderer::process_input() {
    Application::process_input();
}

void SimpleRenderer::framebuffer_size_callback(int width, int height) {
    Application::framebuffer_size_callback(width, height);

    controller_.set_view(45.0f, height);
}

void SimpleRenderer::cursor_pos_callback(double xpos, double ypos) {
    Application::cursor_pos_callback(xpos, ypos);

    auto [delta_x, delta_y] = delta_mouse_pos_;
    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        controller_.rotate(delta_x, delta_y);
    } else if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        controller_.translate(delta_x, delta_y);
    }
}

void SimpleRenderer::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);
}

void SimpleRenderer::key_callback(int key, int scancode, int action, int mods) {
    Application::key_callback(key, scancode, action, mods);

    // Escape - exit
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
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
}

void SimpleRenderer::scroll_callback(double xoffset, double yoffset) {
    Application::scroll_callback(xoffset, yoffset);

    controller_.zoom(yoffset);
}
