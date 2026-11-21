# Credits & Licenses

## Third-Party Libraries
1. **GLFW** (Window & Input Management)
   - Author: Camilla Löwy & GLFW Contributors
   - Link: https://www.glfw.org/
   - License: zlib/libpng License

2. **GLAD** (OpenGL 3.3 Core Function Loader)
   - Author: David Herberth
   - Link: https://glad.dav1d.de/
   - License: MIT / Public Domain

3. **GLM (OpenGL Mathematics)**
   - Author: Christophe Riccio (G-Truc Creation)
   - Link: https://github.com/g-truc/glm
   - License: Happy Bunny License / MIT License

## Educational References & Concepts
- **LearnOpenGL** by Joey de Vries (https://learnopengl.com/, License: CC BY-NC 4.0) — referenced for standard modern OpenGL patterns: VAO/VBO/EBO setup, Blinn-Phong lighting equations, Euler-angle FPS camera math, instanced rendering (`glDrawElementsInstanced`), and alpha-blending render order. All shader implementations and C++ classes were written and customized specifically for the *Alchemist's Potion Lab* scene.

## 3D Models, Textures & Fonts
- **3D Meshes:** 100% procedurally generated in C++ (`PrimitiveFactory.h`) using analytical geometry and lathe surface revolution. No external `.obj`/`.glTF` assets were used.
- **Textures:** 100% procedurally generated in C++ (`Texture.h`) and GLSL (`magic_circle.frag`, `particle.frag`) with OpenGL mipmapping. No external image files were used.
- **HUD Font Atlas:** 5×7 procedural bitmap font embedded in `HudOverlay.h` with zero external font files.
