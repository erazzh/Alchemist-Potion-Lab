# Alchemist's Potion Lab (Topic 15)

Real-time interactive 3D OpenGL application built in C++ (OpenGL 3.3 Core Profile) for the **Computer Graphics Fundamentals** Final Project. Brew glowing potions inside a medieval alchemist's laboratory with dynamic liquid mixing, Blinn-Phong multi-light shading, procedural mipmapped textures, Fresnel glass, GPU-instanced particles, and a procedural glowing magic circle.

---

## Features & Course Requirements Coverage

### Mandatory Requirements (R1–R8)
- **R1 — Modern Pipeline:** OpenGL 3.3 Core Profile with VAO/VBO/EBO buffers and custom GLSL vertex/fragment shaders separated into `.vert` and `.frag` files (`phong`, `glass`, `particle`, `magic_circle`, `hud`). Zero fixed-function calls.
- **R2 — 3D Scene Setup:** 10+ distinct 3D objects in an enclosed stone laboratory (wooden desk with legs, lathe-generated round-bottom main flask, dynamic liquid volume + meniscus, iron tripod stand, 3 stoppered reagent vials on a rack, stacked leather books, open parchment tome, 3 candles with flickering flames, wall shelf with decorative bottles, and a mechanical stirrer stand).
- **R3 — Transformation Pipeline & Hierarchy:** Custom GLM Model-View-Projection pipeline with a 3-level parent-child hierarchy on the mechanical alchemical stirrer (`Stand Base -> Horizontal Arm -> Rotating Shaft & Stirrer Paddle`).
- **R4 — Lighting & Shading:** Per-fragment Blinn-Phong lighting with ambient, diffuse, specular, distance attenuation, and proper normal matrices across 4 dynamic point lights (3 flickering candles toggleable with `C` + 1 dynamic potion light whose color and intensity track the liquid inside the flask).
- **R5 — Texturing with Mipmapping:** Procedural 2D RGB textures (wood planks, stone masonry, leather book cover, ink parchment) uploaded with `glGenerateMipmap`, trilinear filtering (`GL_LINEAR_MIPMAP_LINEAR`), and per-material UV scaling to avoid stretching.
- **R6 — Interactivity:** Frame-rate independent FPS and Orbit cameras (keyboard + mouse + scroll zoom) plus 5+ scene interactions (ingredient mixing, flask draining, candle toggle, stirrer toggle, recipe book & HUD toggle).
- **R7 — Real-Time Performance:** Runs at 60+ FPS at `1280x720` with a live FPS counter displayed both in the window title and in the on-screen HUD overlay.
- **R8 — Reproducible Build:** Builds directly in Visual Studio (x64) using the course's `packages.config` setup with zero extra external dependencies required on the TA machine.

### Stretch Goals & Wow Moment
1. **Semi-Transparent Glass (Fresnel Shader):** Custom `glass.vert`/`glass.frag` using Schlick's Fresnel approximation and sharp Blinn-Phong specular highlights over transparent glowing liquid.
2. **GPU-Instanced Bubbles, Steam & Sparks:** `ParticleSystem` renders rising bubbles inside the flask, rising steam above the neck, and swirling magic sparks using `glDrawElementsInstanced`.
3. **Glowing Magic Circle:** Procedural SDF-based GLSL shader (`magic_circle.frag`) renders a rotating runic hexagram on the desk when a valid potion recipe (3+ drops) is brewed.
4. **Wow Moment:** Brewing a recipe (e.g., *Emerald Flame* or *Void Star Elixir*), pressing `C` to extinguish all candles, and activating auto-orbit (`O`) as the potion and magic circle illuminate the dark room.

---

## Controls

| Input | Action |
| :--- | :--- |
| `W` `A` `S` `D` | Move camera (FPS mode) / Orbit & zoom (Orbit mode) |
| `Q` / `E` | Move down / up (FPS) or adjust pitch (Orbit) |
| Mouse / Scroll | Look around / Orbit & zoom distance |
| `1` / `2` / `3` | Add Red / Blue / Emerald ingredient drop (mixes color & raises level) |
| `R` | Empty the main flask |
| `C` | Toggle candle lights On / Off (Dark Room Wow Moment) |
| `M` | Toggle mechanical stirrer rotation (Parent-Child Hierarchy) |
| `V` | Switch camera mode (`FPS` / `ORBIT`) |
| `O` | Toggle smooth cinematic Auto-Orbit around the flask |
| `H` | Toggle full HUD overlay (compact FPS counter stays visible) |
| `TAB` | Release / Capture mouse cursor |
| `ESC` | Exit application |

### Alchemy Recipe Book (3+ Drops)
- **Void Star Elixir (Purple):** Red + Blue (no Green)
- **Emerald Flame (Neon Green):** 3x Green
- **Dragon Amber (Gold):** Red + Green (no Blue)
- **Astral Frost (Cyan):** Blue + Green (no Red)

---

## Build & Run Instructions (Visual Studio — Windows x64)

1. Clone the repository:
```bash
git clone [<repository-url>](https://github.com/erazzh/Alchemist-Potion-Lab)
cd <repository-folder>
```
2. Open `OpenGL practice.sln` in **Visual Studio 2022**.
3. Right-click the Solution in Solution Explorer and select **Restore NuGet Packages** (restores `GLFW` and `GLM` from `packages.config`).
4. Set the toolbar configuration to **`Release`** (or **`Debug`**) and platform to **`x64`**.
5. Ensure Working Directory is `$(ProjectDir)` (default in VS) so shaders (`.vert` / `.frag`) are loaded from the project folder.
6. Press **`Ctrl + F5`** (*Start Without Debugging*) to build and run.

---

## Project Architecture

- `main.cpp` — Window lifecycle, input callbacks, multi-pass rendering loop (opaque -> translucent liquid -> additive VFX -> Fresnel glass & steam -> 2D HUD).
- `Shader.h` — Separate `.vert`/`.frag` loader with UTF-8 BOM stripping and uniform helpers.
- `Mesh.h` — RAII wrapper for OpenGL VAO/VBO/EBO buffers.
- `PrimitiveFactory.h` — Procedural generators for cubes, cylinders, spheres, disks, planes, and 2D-profile lathe meshes (flasks and rings).
- `Camera.h` — Delta-time independent FPS, Orbit, and cinematic Auto-Orbit camera controller.
- `LightManager.h` — Multi-light controller for harmonically flickering candles and reactive potion glow.
- `Texture.h` — Procedural mipmapped texture generator (wood, stone, leather, parchment).
- `PotionSystem.h` — Weighted color mixing, smooth liquid level interpolation, and recipe evaluation.
- `ParticleSystem.h` — GPU-instanced billboarded particle engine for bubbles, steam, and sparks.
- `HudOverlay.h` — Single-draw-call batched 2D HUD overlay with embedded bitmap font atlas.

---

## Team Members & Contributions

| Member | Group | Responsibilities |
| :--- | :--- | :--- |
| Yeraly Zhumagul | SE-2421 | Core OpenGL pipeline, `Mesh`, `PrimitiveFactory`, scene layout, stirrer hierarchy, integration |
| :--- | :--- | `LightManager`, Blinn-Phong multi-light shaders, candle flicker & dynamic potion glow |
| :--- | :--- | Procedural mipmapped `Texture`, Fresnel glass shader, `PotionSystem` color mixing |
| :--- | :--- | `ParticleSystem` (GPU instancing), `magic_circle` shader, `HudOverlay`, demo video & testing |

---

## AI Use Note

- **Tool used:** Google Gemini (Web).
- **Purpose:** Creating of visualisation image of how the finished project should look, troubleshooting Visual Studio build/compilation issues (C++14 `glm::clamp`, shader file paths, UTF-8 BOM handling, and depth-sorting of translucent liquid and bubbles).
