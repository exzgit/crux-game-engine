#pragma once

#include <glm/glm.hpp>

// Rigidbody component
//
// This component stores physical properties for simple rigid-body
// integration. It's intentionally lightweight: velocity and mass are
// public members so systems can read/write them directly. Documentation
// comments explain semantics and recommended usage.
struct Rigidbody {
  // Linear velocity in world space (meters per second).
  glm::vec3 velocity = glm::vec3(0.0f);

  // Accumulated force (in Newtons) - optional convenience field.
  // Systems may choose to apply forces and then zero this each step.
  glm::vec3 force = glm::vec3(0.0f);

  // Mass in kilograms. A mass value <= 0 is treated as infinite (immovable).
  float mass = 1.0f;

  // Linear damping factor (0..1). Values closer to 1 reduce velocity faster.
  // This is applied as: velocity *= std::max(0.0f, 1.0f - damping * dt)
  float linear_damping = 0.0f;

  // If false, the body is static / kinematic and will not be integrated
  // by the physics system (but can still participate in collision queries).
  bool dynamic = true;

  Rigidbody() = default;
  explicit Rigidbody(float m) : mass(m) {}
};
#pragma once

#include <glm/glm.hpp>

struct RigidBody {
  float mass = 1.0f;
  bool is_static = false;
  float drag = 0.1f;
  float angular_drag = 0.05f;
  float gravity_scale = 1.0f;
  float bounciness = 0.0f;
  float friction = 0.5f;
  float linear_damping = 0.01f;
  float angular_damping = 0.01f;

  RigidBody() = default;
};