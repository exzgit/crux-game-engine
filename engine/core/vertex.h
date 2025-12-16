#pragma once

#include <glm/glm.hpp>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;

  Vertex() = default;
  Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoord)
    : position(position), normal(normal), texCoord(texCoord) {}
};
