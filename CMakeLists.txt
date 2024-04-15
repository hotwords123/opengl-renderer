cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 23)

project(Renderer)

add_definitions(-D_USE_MATH_DEFINES)
add_compile_options(/W4)

include_directories(include)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(openmesh REQUIRED)

add_executable(main
    src/main.cpp src/shader.cpp src/mesh.cpp src/camera.cpp src/control.cpp src/application.cpp src/shadow.cpp
    include/shader.h include/mesh.h include/camera.h include/control.h include/application.h include/shadow.h)

target_include_directories(main PRIVATE ${OPENMESH_INCLUDE_DIRS})

target_link_libraries(main PRIVATE glad::glad)
target_link_libraries(main PRIVATE glfw)
target_link_libraries(main PRIVATE glm::glm)
target_link_libraries(main PRIVATE ${OPENMESH_LIBRARIES})
