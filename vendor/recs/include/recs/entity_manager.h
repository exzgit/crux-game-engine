#pragma once

#include <cstdint>
#include <vector>
#include "entity.h"

class EntityManager {
public:
    EntityManager() = default;
    ~EntityManager() = default;

    Entity create();
    void destroy(Entity e);
    bool is_alive(Entity e) const;
    std::uint32_t alive_count() const noexcept;

private:
    struct Slot {
        std::uint32_t generation;
        bool alive;
    };

    std::vector<Slot> slots;
    std::vector<std::uint32_t> free_list;
    std::uint32_t alive_entities = 0;
};
