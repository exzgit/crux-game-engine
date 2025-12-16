#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <cassert>
#include <new>

#include "component_registry.h"
#include "entity.h"

class Archetype;

#define CONSTANT_CHUNK_SIZE (16 * 1024)

class Chunk {
public:
    static constexpr std::size_t CHUNK_SIZE = CONSTANT_CHUNK_SIZE;

    explicit Chunk(const std::vector<ComponentTypeID>& component_types);
    ~Chunk();

    std::size_t capacity() const noexcept;
    std::size_t size() const noexcept;
    bool full() const noexcept;

    template<typename T>
    T& get(std::size_t row) {
        ComponentTypeID id = ComponentRegistry::instance().type_id<T>();
        return *reinterpret_cast<T*>(component_ptr(id, row));
    }

    template<typename... Components>
    std::tuple<Components*...> get_arrays() {
        return {
            reinterpret_cast<Components*>(
                component_ptr(ComponentRegistry::instance().type_id<Components>(), 0)
            )...
        };
    }

    std::size_t allocate(Entity id);
    Entity deallocate(std::size_t row);

    void move_entity(std::size_t src_row, Chunk& dst, std::size_t& dst_row);

    // Track owning entity per row
    std::vector<Entity> entity_ids;

    void* component_ptr(ComponentTypeID type, std::size_t row);

private:
    struct ComponentLayout {
        std::size_t offset;
        std::size_t stride;
        const ComponentTypeInfo* info;
    };

    void compute_layout(const std::vector<ComponentTypeID>& component_types);

private:
    std::byte* memory = nullptr;
    std::size_t entity_capacity = 0;
    std::size_t entity_count = 0;
    std::unordered_map<ComponentTypeID, ComponentLayout> layouts;
};
