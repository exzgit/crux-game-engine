#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct GridViewport {
  glm::vec3 origin = glm::vec3(0.0f);
  float cell_size = 1.0f;
  int line_count = 10;
};