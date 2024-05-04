#include "scene_demo.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace std;

void error_callback(int error, const char* description) {
    cerr << "Error [" << error << "]: " << description << endl;
}

int main() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    unique_ptr<Application> app;
    app = make_unique<SceneDemo>();

    int ret = app->exec();
    app.reset();

    glfwTerminate();
    return ret;
}
