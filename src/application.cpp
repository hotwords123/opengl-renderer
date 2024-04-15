#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "shadow.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cstdio>
#include <cmath>
#include <utility>

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

static const pair<int, CameraMovement> movement_map[] = {
    {GLFW_KEY_W, CameraMovement::kForward},
    {GLFW_KEY_S, CameraMovement::kBackward},
    {GLFW_KEY_A, CameraMovement::kLeft},
    {GLFW_KEY_D, CameraMovement::kRight},
    {GLFW_KEY_SPACE, CameraMovement::kUp},
    {GLFW_KEY_LEFT_SHIFT, CameraMovement::kDown},
};

Application::Application(GLFWwindow *window)
    : window_(window),
      width_(),
      height_(),
      camera_(),
      controller_(camera_),
      last_time_(0.0f),
      wireframe_(false),
      animating_(true),
      pointer_locked_(true),
      shader_type_(ShaderType::kPhong),
      blinn_(true),
      shadows_(true),
      first_mouse_(true),
      last_x_(),
      last_y_() {
    glfwGetWindowSize(window_, &width_, &height_);

    // Initialize window
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, global_framebuffer_size_callback);
    glfwSetCursorPosCallback(window_, global_cursor_pos_callback);
    glfwSetMouseButtonCallback(window_, global_mouse_button_callback);
    glfwSetKeyCallback(window_, global_key_callback);

    // Capture cursor
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Turn on vsync
    glfwSwapInterval(1);
}

Application::~Application() {
}

int Application::exec() {
    // Load mesh
    BasicMesh mesh;
    if (!mesh.load("test.obj")) {
        cerr << "Failed to load mesh" << endl;
        return 1;
    }
    mesh.setup();

    BasicMesh cube_mesh;
    if (!cube_mesh.load("cube.obj")) {
        cerr << "Failed to load cube mesh" << endl;
        return 1;
    }
    cube_mesh.setup();

    BasicMesh plane_mesh;
    if (!plane_mesh.load("plane.obj")) {
        cerr << "Failed to load plane mesh" << endl;
        return 1;
    }
    plane_mesh.setup();

    // Initialize camera
    camera_.set_position(glm::vec3(250.0f, 250.0f, 250.0f));
    camera_.look_at(glm::vec3(0.0f, 50.0f, 0.0f));

    // Initialize shadow mapping
    PointShadowMap shadow_map(2048, 2048, 1.0f, 1024.0f);

    // Load shader programs
    ShaderProgram phong_shader;
    ShaderProgram gouraud_shader;
    ShaderProgram light_cube_shader;
    ShaderProgram point_shadow_shader;

    phong_shader.build_from_vf("shader/phong");
    gouraud_shader.build_from_vf("shader/gouraud");
    light_cube_shader.build_from_vf("shader/light_cube");
    point_shadow_shader.build_from_vgf("shader/point_shadow");

    glm::vec3 light_pos(200.0f, 150.0f, 0.0f);
    glm::vec3 light_color(1.0f, 1.0f, 1.0f);

    float angle = 0.0f;

    auto render = [&](const ShaderProgram &shader, bool shadow_pass) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
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

        mesh.draw();

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

        plane_mesh.draw();
    };

    // Main loop
    while (!glfwWindowShouldClose(window_)) {
        float time = (float)glfwGetTime();
        float delta_time = time - last_time_;
        last_time_ = time;

        process_input(delta_time);

        if (animating_) {
            angle += 45.0f * delta_time;
        }

        // 1. Render shadow map
        shadow_map.bind();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        point_shadow_shader.use();

        point_shadow_shader.set_vec3("lightPos", light_pos);
        point_shadow_shader.set_float("far", shadow_map.far());
        for (int i = 0; i < 6; i++) {
            string name = "shadowMatrices[" + to_string(i) + "]";
            point_shadow_shader.set_mat4(name.c_str(), shadow_map.shadow_matrix(i));
        }

        render(point_shadow_shader, true);

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

        const ShaderProgram &shader = shader_type_ == ShaderType::kPhong ? phong_shader : gouraud_shader;

        shader.use();

        shader.set_mat4("projection", projection);
        shader.set_mat4("view", view);
        shader.set_vec3("viewPos", camera_.position());

        shader.set_vec3("lightPos", light_pos);
        shader.set_vec3("light.ambient", 0.05f * light_color);
        shader.set_vec3("light.diffuse", 0.85f * light_color);
        shader.set_vec3("light.specular", light_color);
        shader.set_bool("light.blinn", blinn_);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_map.depth_cubemap());
        shader.set_bool("shadows", shadows_);
        shader.set_int("depthCubemap", 0);
        shader.set_float("far", shadow_map.far());

        render(shader, false);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, light_pos);
        model = glm::scale(model, glm::vec3(5.0f));

        light_cube_shader.use();

        light_cube_shader.set_mat4("projection", projection);
        light_cube_shader.set_mat4("view", view);
        light_cube_shader.set_mat4("model", model);
        light_cube_shader.set_vec3("objectColor", light_color);

        cube_mesh.draw();

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    return 0;
}

void Application::process_input(float delta_time) {
    for (const auto &[key, movement] : movement_map) {
        if (glfwGetKey(window_, key) == GLFW_PRESS) {
            controller_.handle_keyboard(movement, delta_time);
        }
    }
}

void Application::framebuffer_size_callback(int width, int height) {
    width_ = width;
    height_ = height;
}

void Application::cursor_pos_callback(double xpos, double ypos) {
    if (!pointer_locked_) {
        return;
    }

    if (first_mouse_) {
        last_x_ = xpos;
        last_y_ = ypos;
        first_mouse_ = false;
        return;
    }

    double xoffset = xpos - last_x_;
    double yoffset = ypos - last_y_;

    last_x_ = xpos;
    last_y_ = ypos;

    controller_.handle_mouse((float)xoffset, (float)yoffset);
}

void Application::mouse_button_callback(int button, int action, int mods) {
    (void)mods;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        pointer_locked_ = true;
        first_mouse_ = true;
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void Application::key_callback(int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

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
