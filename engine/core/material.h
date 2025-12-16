#pragma once

#include <string>
#include <cstdint>
#include <glm/glm.hpp>

struct Material {
  glm::vec3 albedo_color = {0.22f, 0.22f, 0.22f};
  float opacity = 1.0f;

  float metalic = 0.0f;
  float normal = 0.0f;
  float roughness = 0.5f;
  float ao = 0.2f;

  Material() = default;
  Material(uint32_t r, uint32_t g, uint32_t b, uint32_t a) : albedo_color({r, g, b}), opacity(a) {}
};
