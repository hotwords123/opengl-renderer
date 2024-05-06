# OpenGL Renderer

This project implements a simple OpenGL renderer capable of loading OBJ models and rendering them. By default, the renderer loads and renders a demo scene from the `models/` folder. However, if command-line arguments are provided, it will load the specified OBJ model(s), supporting the loading of multiple models simultaneously.

## Features

- Supports Phong and Gouraud shading models.
- Implements Blinn-Phong and Phong lighting models.
- Provides keyboard shortcuts to toggle rendering modes and lighting models.
- Implements Omnidirectional Shadow Mapping, with keyboard shortcuts to enable or disable shadows.

## Usage

Ensure that the working directory contains the necessary files:
- Models should be placed in the `models/` folder.
- Shader code should be placed in the `shader/` folder.

The build process automatically copies these folders to the directory where the executable resides, allowing the program to run from either the project root or the build directory.

### Demo Scene

The demo scene consists of:
- A point light source.
- A face model.
- A ground model.

In the demo scene, the camera adopts a first-person perspective, allowing control of camera movement using the mouse and keyboard:

- **Left mouse button:** Capture/release mouse.
- **Mouse movement:** Control camera view.
- **WASD:** Move camera forward, backward, left, and right.
- **Space:** Move camera upwards.
- **Shift:** Move camera downwards.
- **Esc:** Release mouse capture or exit the program.
- **Tab:** Toggle rendering mode between wireframe and fill.
- **1,2:** Switch shading models between Phong and Gouraud.
- **3,4:** Switch lighting models between Blinn-Phong and Phong.
- **5:** Enable or disable shadows.
- **P:** Pause or resume animation.

### Model Loading

The project supports loading OBJ models. Command-line parameters can specify which models to load.

In the model rendering mode, the camera adopts a third-person perspective, and a trackball is displayed in the center of the screen to assist with positioning. The red, green, and blue planes in the trackball correspond to the $yz$, $xz$, and $xy$ planes, respectively. The light source is fixed at the camera position, eliminating the need to consider shadow effects.

Control camera movement using the mouse and keyboard:

- **Left mouse button:** Adjust camera view.
- **Right mouse button:** Translate camera.
- **Mouse scroll wheel:** Adjust distance between camera and focus.
- **Esc:** Exit the program.
- **Tab:** Toggle rendering mode between wireframe and fill.
- **1,2:** Switch shading models between Phong and Gouraud.
- **3,4:** Switch lighting models between Blinn-Phong and Phong.
- **T:** Show or hide the trackball.

## Acknowledgements

This project is heavily inspired by the tutorials available on [LearnOpenGL](https://learnopengl.com/).

Additionally, this project was developed as a course assignment for _Exploration and Practice in Software Engineering (2)_, under the guidance of [Prof. Feng Xu](http://xufeng.site/).
