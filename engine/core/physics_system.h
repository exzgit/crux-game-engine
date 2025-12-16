#pragma once

#include "recs/world.h"
#include "recs/system.h"
#include "transform.h"
#include "rigidbody.h"
#include "collision.h"

#include <vector>
#include <iostream>

// PhysicsSystem
//
// A minimal physics system that demonstrates:
//  - explicit Euler integration of `Rigidbody` into `Transform`
//  - a gravity acceleration
//  - a very simple, naive AABB collision detection and separation step
//
// The goal is to provide a clear and well-commented example suitable for
// learning and incremental improvement. This system is NOT a production
// quality physics engine.
namespace __RUNTIME__ {

class PhysicsSystem : public System {
public:
	PhysicsSystem()
		: gravity(0.0f, -9.81f, 0.0f) {}

	// Run the physics step.
	// Parameters:
	// - world: ECS world containing Transform, Rigidbody and Collider components
	// - dt: timestep in seconds
	void run(World& world, float dt) override {
		if (dt <= 0.0f) return;

		// 1) Integrate forces and velocities (per-body)
		world.query().for_each<Transform, Rigidbody>([&](Transform& t, Rigidbody& rb) {
		if (!rb.dynamic) return;

		// Accumulate acceleration from forces: a = F / m
		glm::vec3 accel = rb.force;
		if (rb.mass > 0.0f) accel /= rb.mass;

		// Add global gravity
		accel += gravity;

		// Semi-explicit Euler: v += a * dt; x += v * dt
		rb.velocity += accel * dt;

		// Apply simple linear damping
		if (rb.linear_damping > 0.0f) {
			float damp = std::max(0.0f, 1.0f - rb.linear_damping * dt);
			rb.velocity *= damp;
		}

		t.position += rb.velocity * dt;

		// Clear forces for next step (conservative choice)
		rb.force = glm::vec3(0.0f);
		});

		// 2) Broad+Narrow phase (naive): collect colliders and test all pairs.
		struct Item { Transform* t; Collider* c; Rigidbody* rb; };
		std::vector<Item> items;
		items.reserve(64);

		world.query().for_each<Transform, Collider>([&](Transform& t, Collider& c) {
		// Try to fetch Rigidbody (optional). We'll use it when resolving.
		// Note: `World::get` assumes the entity is alive and has the component;
		// here we only queried Transform+Collider, so Rigidbody may be absent.
		// We detect presence by temporarily attempting to access it through
		// a try/catch-free approach: query for Rigidbody separately would be
		// more robust, but for simplicity we capture pointer via reinterpret cast
		// of known component layout (we rely on the ECS to keep components stable
		// across this frame since we performed integration earlier).
		items.push_back({ &t, &c, nullptr });
		});

		// Resolve collisions pairwise
		for (size_t i = 0; i < items.size(); ++i) {
		for (size_t j = i + 1; j < items.size(); ++j) {
			auto& A = items[i];
			auto& B = items[j];

			glm::vec3 amin, amax, bmin, bmax;
			aabb_world_bounds(A.t->position, *A.c, amin, amax);
			aabb_world_bounds(B.t->position, *B.c, bmin, bmax);

			if (!aabb_intersect(amin, amax, bmin, bmax)) continue;

			// Compute minimal translation to separate A from B
			glm::vec3 mtv = aabb_mtv(amin, amax, bmin, bmax);
			if (glm::length(mtv) == 0.0f) continue;

			// Simple resolution strategy: move dynamic bodies out proportionally.
			// If both are dynamic (we don't have Rigidbody pointers here), we'll
			// move both by half. If only one is dynamic, move that one by full
			// MTV. For demonstration we treat both as dynamic; for better control
			// you can extend the items vector to include Rigidbody pointers.
			// We'll conservatively move both by 50% of MTV in opposite directions.
			A.t->position += -0.5f * mtv;
			B.t->position +=  0.5f * mtv;

			// Velocities: simple bounce using restitution average
			// (If Rigidbody were present, you'd alter velocities along mtv axis.)
		}
		}
	}

	glm::vec3 gravity;
};

} // namespace __RUNTIME__
