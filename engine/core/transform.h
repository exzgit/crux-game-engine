#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

struct Transform {
  glm::vec3 position = {0.0f, 0.0f, 0.0f};
  glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
  glm::vec3 scale = {1.0f, 1.0f, 1.0f};

  // Local matrix built from position/rotation/scale (local space)
  glm::mat4 local = glm::mat4(1.0f);
  // World transform after applying parent transforms
  glm::mat4 world = glm::mat4(1.0f);

  // Mark when local changed and world needs recompute
  bool dirty = true;

  Transform() = default;
  Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl)
    : position(pos), rotation(rot), scale(scl) {
    dirty = true;
  }

  // Rebuild local matrix from TRS
  void rebuild_local() {
    glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::rotate(m, glm::radians(rotation.x), glm::vec3(1,0,0));
    m = glm::rotate(m, glm::radians(rotation.y), glm::vec3(0,1,0));
    m = glm::rotate(m, glm::radians(rotation.z), glm::vec3(0,0,1));
    m = glm::scale(m, scale);
    local = m;
    dirty = true;
  }
};

struct Velocity {
  float x = 0;
  float y = 0;
  float z = 0;

  Velocity() {}
  Velocity(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};
