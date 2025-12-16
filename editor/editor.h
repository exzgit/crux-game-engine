#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include <optional>
#include <cstdint>
#include <recs/entity.h>

#include <recs/world.h>
#include <engine/core/physics_system.h>
#include <engine/core/render.h>
#include <engine/core/camera.h>
#include <engine/tools/load_object.h>
#include <engine/core/time.h>
#include <engine/core/transform.h>
#include <engine/core/mesh.h>
#include <engine/core/input.h>
#include <recs/entity.h>

namespace __EDITOR__ {
//
// Callback function for window resize events.
// Ensures OpenGL viewport matches the new window dimensions.
//
// Parameters:
//  window    - pointer to the GLFW window.
//  width     - new width of the framebuffer.
//    height    - new height of the framebuffer.
//
void framebuffer_size_callback(GLFWwindow*, int width, int height);

enum class EditorMode {
    Select,
    Translate,
    Rotate,
    Scale
};

struct EditorState {
    // ==== Selection ====
    std::optional<Entity> selected_entity;

    // ==== Viewport ====
    bool viewport_hovered = false;
    bool viewport_focused = false;
    glm::vec2 viewport_size = {0.0f, 0.0f};
    glm::vec2 viewport_pos  = {0.0f, 0.0f};
    uint32_t viewport_texture = 0; // OpenGL texture ID
    
    // ==== Camera control ====
    bool camera_control_active = false;

    // ==== Gizmo ====
    EditorMode gizmo_mode = EditorMode::Select;
    bool gizmo_local_space = false;

    // ==== Play state ====
    bool play_mode = false;

    // ==== Helpers ====
    inline void clear_selection() {
        selected_entity.reset();
    }

    inline bool has_selection() const {
        return selected_entity.has_value();
    }
};

class CruxEditor {
  public:
  CruxEditor();
  ~CruxEditor();

  void update();

  private:
  GLFWwindow* window;

  void camera_editor(Transform& transform, Camera& camera, __RUNTIME__::SystemRenderer& renderer);

  void draw_editor_dockspace();
  void draw_main_menu_bar();
  void draw_viewport(EditorState& state);
  void draw_hierarchy(World& world, Entity* selected);
};

};
