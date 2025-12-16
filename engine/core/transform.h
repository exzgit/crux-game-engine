#pragma once

#include "glm/glm.hpp"

struct Transform {
  glm::vec3 position = {0.0f, 0.0f, 0.0f};
  glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
  glm::vec3 scale = {1.0f, 1.0f, 1.0f};

  Transform() = default;
  Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
    : position(pos), rotation(rot), scale(scale) {}
};

struct Velocity {
  float x = 0;
  float y = 0;
  float z = 0;

  Velocity() {}
  Velocity(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};
