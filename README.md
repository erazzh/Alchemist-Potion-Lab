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
- **Emerald Flame (Neon Green):** 3× Green
- **Dragon Amber (Gold):** Red + Green (no Blue)
- **Astral Frost (Cyan):** Blue + Green (no Red)

---

## Build & Run Instructions (Visual Studio — Windows x64)

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd <repository-folder>
