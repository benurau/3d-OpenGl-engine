# 3D OpenGL Engine

A lightweight and modular 3D graphics engine built with OpenGL and C++.  
This project focuses on real-time rendering, graphics programming fundamentals, and engine architecture design for interactive 3D applications.

---

## Preview

> Add screenshots or GIFs here to showcase the engine.

Example:

```md
![Engine Demo](demo.gif)
```

---

## Features

- Real-time 3D rendering
- OpenGL-based graphics pipeline
- Camera movement and transformations
- Shader system
- Model loading support
- Lighting system
- Texture mapping
- Basic scene management
- Cross-platform C++ architecture
- Modern OpenGL practices

---

## Tech Stack

- **Language:** C++
- **Graphics API:** OpenGL
- **Windowing/Input:** GLFW
- **OpenGL Loader:** GLEW / GLAD
- **Math Library:** GLM
- **Build System:** CMake

---

## Project Structure

```bash
3d-OpenGl-engine/
│
├── src/               # Engine source files
├── shaders/           # GLSL shader programs
├── assets/            # Models, textures, resources
├── include/           # Header files
├── libs/              # External libraries
├── CMakeLists.txt
└── README.md
```

---

## Getting Started

### Prerequisites

Make sure you have installed:

- C++17 or newer
- OpenGL 3.3+
- CMake
- GLFW
- GLM
- GLEW / GLAD

---

## Installation

Clone the repository:

```bash
git clone https://github.com/benurau/3d-OpenGl-engine.git
cd 3d-OpenGl-engine
```

Build the project:

```bash
mkdir build
cd build
cmake ..
make
```

Run the engine:

```bash
./3d-OpenGl-engine
```

---

## Controls

| Key | Action |
|------|--------|
| W / A / S / D | Move camera |
| Mouse | Look around |
| ESC | Exit application |

---

## Goals of the Project

This engine was created to:

- Learn low-level graphics programming
- Understand rendering pipelines
- Experiment with engine architecture
- Explore shader programming
- Build a foundation for future game development

---

## Future Improvements

- [ ] Physically Based Rendering (PBR)
- [ ] Shadow Mapping
- [ ] ECS Architecture
- [ ] Animation System
- [ ] Deferred Rendering
- [ ] Post-processing Effects
- [ ] GUI Editor
- [ ] Vulkan Renderer Backend

---

## Learning Resources

Some inspiration and reference material for OpenGL engine development:

- LearnOpenGL — https://learnopengl.com
- OpenGL Documentation — https://www.opengl.org/documentation/
- GLFW Documentation — https://www.glfw.org/documentation.html
- GLM GitHub Repository — https://github.com/g-truc/glm

---

## Contributing

Contributions, suggestions, and feedback are welcome.

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Open a pull request

---

## License

This project is licensed under the MIT License.

---

## Acknowledgements

This project was inspired by various OpenGL and graphics programming projects from the community and educational resources focused on real-time rendering and engine development.
