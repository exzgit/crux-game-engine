#pragma once

#include "../core/mesh.h"
#include "../core/vertex.h"
#include "../core/transform.h"
#include "../core/material.h"
#include "recs/world.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

namespace __TOOLS__ {

static void load_obj_mesh(std::ifstream& file, Mesh& mesh, std::string& name) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;

    mesh.vertices.clear();
    mesh.indices.clear();

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        // ============================
        // Read mesh name
        // ============================

        if (prefix == "o" || prefix == "g") {
            ss >> name;
            continue;
        }

        // =====================
        // Vertex position
        // =====================
        if (prefix == "v") {
            glm::vec3 p;
            ss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }

        // =====================
        // Texture coordinate
        // =====================
        else if (prefix == "vt") {
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            texCoords.push_back(uv);
        }

        // =====================
        // Normal
        // =====================
        else if (prefix == "vn") {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(glm::normalize(n));
        }

        // =====================
        // Face
        // =====================
        else if (prefix == "f") {
            std::vector<uint32_t> faceVertexIndices;

            std::string token;
            while (ss >> token) {
                uint32_t pi = 0, ti = UINT32_MAX, ni = UINT32_MAX;

                std::istringstream ts(token);
                std::string part;

                // position index
                std::getline(ts, part, '/');
                pi = static_cast<uint32_t>(std::stoi(part)) - 1;

                // texcoord index (optional)
                if (std::getline(ts, part, '/')) {
                    if (!part.empty())
                        ti = static_cast<uint32_t>(std::stoi(part)) - 1;
                }

                // normal index (optional)
                if (std::getline(ts, part, '/')) {
                    if (!part.empty())
                        ni = static_cast<uint32_t>(std::stoi(part)) - 1;
                }

                Vertex v;
                v.position = positions[pi];
                v.texCoord = (ti != UINT32_MAX && ti < texCoords.size())
                    ? texCoords[ti]
                    : glm::vec2(0.0f);

                if (ni != UINT32_MAX && ni < normals.size()) {
                    v.normal = normals[ni];
                } else {
                    v.normal = glm::vec3(0.0f); // placeholder, will be computed
                }

                uint32_t index = static_cast<uint32_t>(mesh.vertices.size());
                mesh.vertices.push_back(v);
                faceVertexIndices.push_back(index);
            }

            // === Triangulate face (fan method) ===
            for (size_t i = 1; i + 1 < faceVertexIndices.size(); ++i) {
                mesh.indices.push_back(faceVertexIndices[0]);
                mesh.indices.push_back(faceVertexIndices[i]);
                mesh.indices.push_back(faceVertexIndices[i + 1]);
            }
        }
    }

    // =========================================================
    // NORMAL GENERATION (only if OBJ did not provide normals)
    // =========================================================
    bool needsNormals = true;
    for (const auto& v : mesh.vertices) {
        if (glm::length(v.normal) > 0.0f) {
            needsNormals = false;
            break;
        }
    }

    if (needsNormals && !mesh.indices.empty()) {
        // reset normals
        for (auto& v : mesh.vertices)
            v.normal = glm::vec3(0.0f);

        // accumulate face normals
        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
            Vertex& v0 = mesh.vertices[mesh.indices[i + 0]];
            Vertex& v1 = mesh.vertices[mesh.indices[i + 1]];
            Vertex& v2 = mesh.vertices[mesh.indices[i + 2]];

            glm::vec3 e1 = v1.position - v0.position;
            glm::vec3 e2 = v2.position - v0.position;
            glm::vec3 fn = glm::normalize(glm::cross(e1, e2));

            v0.normal += fn;
            v1.normal += fn;
            v2.normal += fn;
        }

        // normalize vertex normals
        for (auto& v : mesh.vertices) {
            if (glm::length(v.normal) > 0.0f)
                v.normal = glm::normalize(v.normal);
            else
                v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}


static Entity create_entity_from_mesh(World& world, const Mesh& mesh, std::string name) {
    Entity entity = world.create_entity();
    assert(world.alive(entity));
    world.add<Transform>(entity);
    world.add<Mesh>(entity);
    world.get<Mesh>(entity) = mesh;
    
    // center mesh vertices around origin (model-space) so Transform.position controls world placement
    if (!world.get<Mesh>(entity).vertices.empty()) {
        glm::vec3 centroid(0.0f);
        for (const auto& v : world.get<Mesh>(entity).vertices) centroid += v.position;
        centroid /= static_cast<float>(world.get<Mesh>(entity).vertices.size());
        for (auto& v : world.get<Mesh>(entity).vertices) v.position -= centroid;
    }

    // add a material
    world.add<Material>(entity);

    // add an identity so every entity has a name/tag/layer
    world.add<Identity>(entity);
    world.get<Identity>(entity) = Identity(name);

    // Construct MeshRenderer in-place using the (now centered) mesh reference
    // Do this after adding Material/Identity to avoid extra moves that can
    // complicate component addresses during creation.
    world.emplace<MeshRenderer>(entity, world.get<Mesh>(entity));

    // ensure Transform starts at origin (mesh already centered)
    world.get<Transform>(entity).position = {0.0f, 0.0f, 0.0f};
    // debug: print first vertex for diagnosis
    if (!world.get<Mesh>(entity).vertices.empty()) {
        const auto& fv = world.get<Mesh>(entity).vertices.front();
        std::cerr << "[loader] first vertex pos (centered): (" << fv.position.x << ", " << fv.position.y << ", " << fv.position.z << ")\n";
    }
    return entity;
}

static Entity create_entities_from_obj(World& world, const std::string& filepath) {
    std::vector<Entity> entities;

    std::ifstream file(filepath);
    if (!file.is_open()) {
        Entity e = world.create_entity();
        assert(world.alive(e));
        world.add<Transform>(e);
        std::cerr << "[I/O ERROR] ==> Failed to open file: " << filepath << "\n";
        return e;
    }

    while (file.peek() != EOF) {
        Mesh mesh;
        std::string name;
        load_obj_mesh(file, mesh, name);
        if (!mesh.vertices.empty()) {
            auto entity = create_entity_from_mesh(world, mesh, name);
            assert(world.alive(entity));
            entities.push_back(entity);
        }
    }

    file.close();
        if (entities.empty()) {
            auto e = world.create_entity();
            assert(world.alive(e));
            world.add<Transform>(e);
            world.add<Identity>(e);
            world.get<Identity>(e) = Identity("EmptyObject");
            return e;
        } else if (entities.size() > 1) {
            auto e = world.create_entity();
            assert(world.alive(e));
            world.add<Transform>(e);
            std::filesystem::path p(filepath);
            
            // add family and identity to parent
            world.add<Family>(e);
            world.add<Identity>(e);
            world.get<Identity>(e).name = p.filename().string();

            // set parent identity name from filepath (basename)
            std::string base = filepath;
            auto pos = base.find_last_of("/\\");
            if (pos != std::string::npos) base = base.substr(pos + 1);
            world.get<Identity>(e) = Identity(base);

                for (auto child : entities) {
                    // set child's parent to the parent entity
                    world.get<Family>(child).parent = e;
                    // add child to parent's children list
                    world.get<Family>(e).children.push_back(child);
                    // MeshRenderer already created in create_entity_from_mesh; avoid
                    // emplacing again here to prevent unnecessary moves.
                }

            return e;
        }


    // MeshRenderer already emplaced in create_entity_from_mesh; no-op here.
    return entities[0];
}
}; // namespace __TOOLS__
