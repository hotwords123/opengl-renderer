#include "scene_demo.h"
#include "simple_renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace std;

void error_callback(int error, const char* description) {
    cerr << "Error [" << error << "]: " << description << endl;
}

int main(int argc, char* argv[]) {
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
    if (argc > 1) {
        // OBJ files are provided, launch simple renderer
        vector<string> obj_files;
        obj_files.reserve(argc - 1);
        for (int i = 1; i < argc; ++i) {
            obj_files.emplace_back(argv[i]);
        }
        app = make_unique<SimpleRenderer>(std::move(obj_files));
    } else {
        // Otherwise, launch scene demo
        app = make_unique<SceneDemo>();
    }

    int ret = app->exec();
    app.reset();

    glfwTerminate();
    return ret;
}
