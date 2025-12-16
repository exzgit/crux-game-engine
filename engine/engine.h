#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <recs/world.h>
#include <string>

#include "core/error.h"
#include "core/input.h"
#include "core/physics_system.h"
#include "core/render.h"
#include "core/time.h"

namespace __ENGINE__ {

#define DEFAULT_SCREEN_WIDTH    800
#define DEFAULT_SCREEN_HEIGHT   600

class CruxEngine {
  public:
  std::string title = "Crux Engine";

  CruxEngine();
  ~CruxEngine();

  void init();
  void update();

  World* get_world_ptr() { return &world; }
  GLFWwindow* get_window_ptr() { return window; }
  __RUNTIME__::SystemRenderer* get_renderer_ptr() { return &renderer; }

  private:
  GLFWwindow* window;
  __RUNTIME__::SystemRenderer renderer = __RUNTIME__::SystemRenderer();
  //
  // Create the main ECS world instance.
  // The World manages all entities, components, and systems.
  //
  World world = World();
};

};
