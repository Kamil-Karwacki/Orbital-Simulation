#  Orbital Simulation (C++ / OpenGL)

A real-time 3D orbital simulation of a solar system (N-body problem) built with C++ and OpenGL. The project features a fully interactive UI to create, modify, and observe planetary bodies in space.

##  Features
* **Real-time N-Body Physics:** Accurate gravitational calculations between multiple celestial bodies.
* **Procedural Planetary Generation:** Uses Perlin noise to dynamically generate planet surfaces and terrain variations.
* **Trajectory Prediction:** Visualizes the future paths of planets before you place them.
* **Interactive UI (Dear ImGui):** Seamlessly add new planets, adjust mass, velocity and customize planet colors on the fly.
* **Advanced Rendering:** Custom shaders featuring a multi-pass Bloom effect and dynamic lighting based on planetary emissions.
* **Scene saving/loading:** Save and load scenes containing existing planets.

##  Technologies & Libraries
* **Language:** C++17
* **Graphics API:** OpenGL 3.3+
* **Libraries:**
  * [GLFW](https://www.glfw.org/) (Window & Input)
  * [GLAD](https://glad.dav1d.de/) (OpenGL function loader)
  * [GLM](https://glm.g-truc.net/) (Mathematics)
  * [Dear ImGui](https://github.com/ocornut/imgui) (Graphical User Interface)

##  How to Build & Run

Thanks to CMake `FetchContent`, all dependencies are downloaded and compiled automatically. 

### Prerequisites
* A C++17 compatible compiler (MSVC, GCC, or Clang)
* CMake (3.15 or newer)
* Git

### Build Steps

1. **Clone the repository:**
   ```bash
   git clone (https://github.com/Kamil-Karwacki/Orbital-Simulation.git)
   cd Orbital-Simulation
   ```

2. **Generate the build files:**
   ```bash
   cmake -B build
   ```

3. **Compile the project:**
   ```bash
   cmake --build build --config Release
   ```

4. **Run the simulation:**
   The executable will be generated in the `build` directory (or `build/Release` on Windows).
   ```bash
   ./build/Orbital-Simulation
   ```

##  Controls
* **Mouse / Keyboard:**
  * Use WASD keys to move across the universe.
  * Hold mouse wheel to rotate the camera around.
  * Press on a planet with LMB to change its properties.
* **UI Panel:** Use the on-screen ImGui panel to spawn planets, customize them, and control the time scale.
