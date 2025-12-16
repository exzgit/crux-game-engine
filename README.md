# Crux Game Engine

Crux Game Engine is a lightweight, modular game engine designed for small- to mid-sized 2D/3D game development. The project emphasizes simplicity, fast build times, and quick iteration — a great fit for indie game studios and solo developers who want a lean development workflow for prototypes and final builds.

**Key features:**
- Modular structure: engine, editor, and ECS (`recs`) are separated
- OpenGL rendering with `glad` and `glfw`
- Math utilities via `glm`
- Simple build using `meson` + `ninja`

**Who this is for:**
This engine suits indie teams or solo developers who need:
- A lightweight, maintainable codebase
- Fast iteration during code and asset changes
- A minimal foundation that's easy to customize and extend

---

Requirements (Linux, Debian/Ubuntu example):
- Meson (>= 0.50)
- Ninja
- A C++ compiler with C++20 support (modern GCC or Clang)
- `pkg-config`
- Development packages for GLFW and GLM (package names may vary by distro)

Install example for Debian/Ubuntu:

```bash
sudo apt update
sudo apt install build-essential meson ninja-build pkg-config libglfw3-dev libglm-dev
```

If you use a different distribution, install the development packages for `glfw` and `glm` using your package manager or build them from source.

---

Build instructions

1. Configure the build directory with Meson:

```bash
meson setup build --buildtype=debug
```

2. Build the project:

```bash
meson compile -C build
```

3. Run the binaries:

```bash
./build/CruxEngine
./build/CruxEditor
```

To build a release version:

```bash
meson setup build --reconfigure --buildtype=release
meson compile -C build
```

Notes: the project bundles `glad` in `vendor/`, while `glfw3` and `glm` are resolved via system dependencies through Meson.

---

Project structure (summary):
- `engine/` — runtime engine code and the `CruxEngine` executable
- `editor/` — sample editor and the `CruxEditor` executable
- `vendor/glad/` — bundled OpenGL loader
- `vendor/recs/` — small archetype-based ECS implementation
- `meson.build` — build configuration

Contributing
- Bug reports, feature requests, and PRs are welcome. Follow the existing C++20 coding style and include small examples or tests when possible.

License
- Check for a `LICENSE` file in this repository. If none is present, contact the repository owner for licensing details before commercial use.

---

Would you like me to add platform-specific build instructions for macOS/Windows, a `launch.json`/`tasks.json` for VSCode, or a simple CI script to run builds automatically? If so, which one should I add first?

