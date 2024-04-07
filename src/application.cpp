#include "application.h"

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

static void global_mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    get_app(window)->mouse_callback(xpos, ypos);
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
      camera_(
          glm::vec3(0.0f),
          glm::vec3(0.0f, 1.0f, 0.0f),
          0.0f,
          0.0f
      ),
      controller_(camera_),
      last_time_(0.0f),
      wireframe_(false),
      shader_type_(ShaderType::kPhong),
      blinn_(true),
      first_mouse_(true),
      last_x_(),
      last_y_() {
    glfwGetWindowSize(window_, &width_, &height_);

    // Initialize window
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, global_framebuffer_size_callback);
    glfwSetCursorPosCallback(window_, global_mouse_callback);

    // Capture cursor
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

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

    // Initialize camera
    camera_.set_position(glm::vec3(150.0f, 50.0f, -250.0f));
    camera_.look_at(glm::vec3(0.0f));

    // Load shader program
    ShaderProgram phong_shader;
    ShaderProgram gouraud_shader;
    ShaderProgram light_cube_shader;

    phong_shader.build_from("shader/phong.vs", "shader/phong.fs");
    gouraud_shader.build_from("shader/gouraud.vs", "shader/gouraud.fs");
    light_cube_shader.build_from("shader/light_cube.vs", "shader/light_cube.fs");

    float fov = glm::radians(45.0f);
    float aspect = (float)width_ / height_;
    glm::mat4 projection = glm::perspective(fov, aspect, 0.1f, 1000.0f);

    // Main loop
    while (!glfwWindowShouldClose(window_)) {
        float time = (float)glfwGetTime();
        float delta_time = time - last_time_;
        last_time_ = time;

        process_input(delta_time);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (wireframe_) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glm::mat4 view = camera_.view_matrix();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(45.0f * time), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -glm::vec3(300.0f, 150.0f, 0.0f));

        glm::mat3 normal_matrix(glm::transpose(glm::inverse(view * model)));

        glm::vec3 object_color(1.0f, 0.5f, 0.31f);
        glm::vec3 light_pos(-50.0f, 50.0f, -200.0f);
        glm::vec3 light_color(1.0f, 1.0f, 1.0f);

        const ShaderProgram &shader = shader_type_ == ShaderType::kPhong ? phong_shader : gouraud_shader;
        shader.use();

        shader.set_mat4("model", model);
        shader.set_mat4("view", view);
        shader.set_mat4("projection", projection);
        shader.set_mat3("normalMatrix", normal_matrix);
        shader.set_vec3("lightPos", light_pos);

        shader.set_vec3("material.ambient", object_color);
        shader.set_vec3("material.diffuse", object_color);
        shader.set_vec3("material.specular", glm::vec3(0.5f));
        shader.set_float("material.shininess", blinn_ ? 16.0f : 8.0f);
        shader.set_vec3("light.ambient", 0.1f * light_color);
        shader.set_vec3("light.diffuse", 0.8f * light_color);
        shader.set_vec3("light.specular", light_color);
        shader.set_bool("light.blinn", blinn_);

        mesh.draw();

        model = glm::mat4(1.0f);
        model = glm::translate(model, light_pos);
        model = glm::scale(model, glm::vec3(5.0f));

        light_cube_shader.use();

        light_cube_shader.set_mat4("model", model);
        light_cube_shader.set_mat4("view", view);
        light_cube_shader.set_mat4("projection", projection);

        light_cube_shader.set_vec3("objectColor", light_color);

        cube_mesh.draw();

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    return 0;
}

void Application::process_input(float delta_time) {
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }

    wireframe_ = glfwGetKey(window_, GLFW_KEY_TAB) == GLFW_PRESS;

    if (glfwGetKey(window_, GLFW_KEY_1) == GLFW_PRESS) {
        shader_type_ = ShaderType::kPhong;
    }
    if (glfwGetKey(window_, GLFW_KEY_2) == GLFW_PRESS) {
        shader_type_ = ShaderType::kGouraud;
    }

    if (glfwGetKey(window_, GLFW_KEY_3) == GLFW_PRESS) {
        blinn_ = true;
    }
    if (glfwGetKey(window_, GLFW_KEY_4) == GLFW_PRESS) {
        blinn_ = false;
    }

    for (const auto &[key, movement] : movement_map) {
        if (glfwGetKey(window_, key) == GLFW_PRESS) {
            controller_.handle_keyboard(movement, delta_time);
        }
    }
}

void Application::framebuffer_size_callback(int width, int height) {
    width_ = width;
    height_ = height;
    glViewport(0, 0, width, height);
}

void Application::mouse_callback(double xpos, double ypos) {
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
