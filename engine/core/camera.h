#pragma once

#include <glm/glm.hpp>

enum class CameraPerspective {
  Perspective,
  Orthographic
};

enum class CameraOrigin {
  World,
  Viewport
};

struct Camera {
  CameraPerspective type = CameraPerspective::Perspective;
  CameraOrigin origin = CameraOrigin::World;

  float fov = 60.0f;
  float aspect_ratio = 4.0f / 3.0f;
  float near_plane = 0.1f;
  float far_plane = 100.0f;

  float yaw = -90.0f;
  float pitch = 0.0f;

  glm::vec3 forward = {0.0f, 0.0f, -1.0f};
  glm::vec3 right = {1.0f, 0.0f, 0.0f};
  glm::vec3 up = {0.0f, 1.0f, 0.0f};

  Camera() = default;
};
