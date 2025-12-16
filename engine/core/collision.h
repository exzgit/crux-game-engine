#pragma once

#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>

// Collider component (Axis-Aligned Bounding Box)
//
// This header defines a simple AABB collider component suitable for basic
// collision detection in demonstration and prototyping. Each entity that
// should participate in collisions can add a `Collider` component and
// provide a `half_extents` vector describing the box extents in model/local
// space. During collision tests we transform the AABB by the entity's
// `Transform::position` (no rotation applied for AABB simplicity).
//
// Note: This is intentionally minimal. For production use you would want
// oriented boxes, collision layers, continuous collision, and a proper
// broadphase (e.g. spatial hashing or BVH).

struct Collider {
	// Half extents of the AABB in local/model space (positive values).
	glm::vec3 half_extents = glm::vec3(0.5f);

	// Optional collision restitution (0..1). Used to scale velocity after
	// collision response. 0 = inelastic, 1 = elastic.
	float restitution = 0.0f;

	// Collision layer mask (bitmask). Systems may use this to filter which
	// colliders should interact. Default 0xFFFFFFFF interacts with everything.
	uint32_t layer_mask = 0xFFFFFFFFu;
};

// Compute world-space AABB min/max from transform position and collider half-extents.
inline void aabb_world_bounds(const glm::vec3& pos, const Collider& c, glm::vec3& out_min, glm::vec3& out_max) {
	out_min = pos - c.half_extents;
	out_max = pos + c.half_extents;
}

// Test AABB intersection between two world-space AABBs.
inline bool aabb_intersect(const glm::vec3& amin, const glm::vec3& amax, const glm::vec3& bmin, const glm::vec3& bmax) {
	return (amin.x <= bmax.x && amax.x >= bmin.x) &&
				 (amin.y <= bmax.y && amax.y >= bmin.y) &&
				 (amin.z <= bmax.z && amax.z >= bmin.z);
}

// Compute minimal translation vector (MTV) to separate two overlapping AABBs.
// Returns vector to move 'a' out of collision with 'b'. If there's no overlap,
// returns a zero vector.
inline glm::vec3 aabb_mtv(const glm::vec3& amin, const glm::vec3& amax, const glm::vec3& bmin, const glm::vec3& bmax) {
	if (!aabb_intersect(amin, amax, bmin, bmax)) return glm::vec3(0.0f);

	// Compute overlap on each axis
	float ox = std::min(amax.x, bmax.x) - std::max(amin.x, bmin.x);
	float oy = std::min(amax.y, bmax.y) - std::max(amin.y, bmin.y);
	float oz = std::min(amax.z, bmax.z) - std::max(amin.z, bmin.z);

	// Find smallest axis to separate
	if (ox <= oy && ox <= oz) {
		// X axis smallest
		float dir = ((amin.x + amax.x) * 0.5f) < ((bmin.x + bmax.x) * 0.5f) ? -1.0f : 1.0f;
		return glm::vec3(dir * ox, 0.0f, 0.0f);
	} else if (oy <= ox && oy <= oz) {
		float dir = ((amin.y + amax.y) * 0.5f) < ((bmin.y + bmax.y) * 0.5f) ? -1.0f : 1.0f;
		return glm::vec3(0.0f, dir * oy, 0.0f);
	} else {
		float dir = ((amin.z + amax.z) * 0.5f) < ((bmin.z + bmax.z) * 0.5f) ? -1.0f : 1.0f;
		return glm::vec3(0.0f, 0.0f, dir * oz);
	}
}
