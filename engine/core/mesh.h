#pragma once

#include "vertex.h"

#include <vector>
#include <cstdint>
#include <iostream>

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<std::uint32_t> indices;

  Mesh() = default;
  Mesh(const std::vector<Vertex>& verts, const std::vector<std::uint32_t>& inds)
    : vertices(verts), indices(inds) {}
};

struct MeshRenderer {
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;
    std::uint32_t index_count = 0;

    MeshRenderer(const Mesh& mesh) {
        index_count = static_cast<std::uint32_t>(mesh.indices.size());

        // Generate GPU buffers
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            mesh.vertices.size() * sizeof(Vertex),
            mesh.vertices.data(),
            GL_STATIC_DRAW
        );

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            mesh.indices.size() * sizeof(std::uint32_t),
            mesh.indices.data(),
            GL_STATIC_DRAW
        );

        // Vertex attributes
        // layout(location = 0) vec3 position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, position))
        );

        // layout(location = 1) vec3 normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, normal))
        );

        // layout(location = 2) vec2 texcoord
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, texCoord))
        );

        glBindVertexArray(0);
    }

    void draw() const {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    ~MeshRenderer() {
        if (ebo) glDeleteBuffers(1, &ebo);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (vao) glDeleteVertexArrays(1, &vao);
    }
};
