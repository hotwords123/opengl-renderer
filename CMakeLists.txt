cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 23)

project(Renderer)

add_definitions(-D_USE_MATH_DEFINES)
add_compile_options(/W4)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(openmesh REQUIRED)

set(SOURCES
    src/main.cpp
    src/shader.cpp
    src/mesh.cpp
    src/camera.cpp
    src/control.cpp
    src/application.cpp
    src/shadow.cpp
    src/scene_demo.cpp
    src/simple_renderer.cpp
    src/shape.cpp
)

set(HEADERS
    include/shader.h
    include/mesh.h
    include/camera.h
    include/control.h
    include/application.h
    include/shadow.h
    include/scene_demo.h
    include/simple_renderer.h
    include/shape.h
)

file(COPY shader DESTINATION "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
file(COPY models DESTINATION "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

add_executable(main ${SOURCES} ${HEADERS})

include_directories(include)

target_include_directories(main PRIVATE ${OPENMESH_INCLUDE_DIRS})

target_link_libraries(main PRIVATE glad::glad)
target_link_libraries(main PRIVATE glfw)
target_link_libraries(main PRIVATE glm::glm)
target_link_libraries(main PRIVATE ${OPENMESH_LIBRARIES})
