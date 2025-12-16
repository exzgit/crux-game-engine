#include <cassert>
#include <iostream>

#include "recs/world.h"
#include "components.h"

static void test_entity_lifecycle() {
    World world;

    Entity e = world.create_entity();
    assert(world.alive(e));

    world.destroy_entity(e);
    assert(!world.alive(e));
}

static void test_add_get_component() {
    World world;
    Entity e = world.create_entity();

    world.add<Position>(e);
    world.add<Velocity>(e);

    auto& pos = world.get<Position>(e);
    auto& vel = world.get<Velocity>(e);

    pos.x = 1.0f;
    pos.y = 2.0f;
    vel.x = 0.5f;
    vel.y = 1.0f;

    assert(world.get<Position>(e).x == 1.0f);
    assert(world.get<Velocity>(e).y == 1.0f);
}

static void test_query_iteration() {
    World world;

    Entity e1 = world.create_entity();
    Entity e2 = world.create_entity();
    Entity e3 = world.create_entity();

    world.add<Position>(e1);
    world.add<Velocity>(e1);

    world.add<Position>(e2);
    world.add<Velocity>(e2);

    world.add<Position>(e3); // no velocity

    world.get<Position>(e1) = {0, 0};
    world.get<Velocity>(e1) = {1, 0};

    world.get<Position>(e2) = {0, 0};
    world.get<Velocity>(e2) = {0, 1};

    std::size_t count = 0;

    bool called = false;

    world.query().for_each<Position, Velocity>(
        [&](Position& p, Velocity& v) {
            called = true;
            ++count;
            p.x += v.x;
            p.y += v.y;
        }
    );

    assert(count == 2);
    assert(world.get<Position>(e1).x == 1.0f);
    assert(world.get<Position>(e2).y == 1.0f);
}

static void test_archetype_migration() {
    World world;

    Entity e = world.create_entity();
    world.add<Position>(e);

    world.get<Position>(e) = {5, 5};

    // structural change
    world.add<Velocity>(e);

    auto& pos = world.get<Position>(e);
    auto& vel = world.get<Velocity>(e);

    assert(pos.x == 5);
    assert(pos.y == 5);

    vel.x = 2;
    vel.y = 3;

    assert(world.get<Velocity>(e).y == 3);
}

int main() {
    std::cout << "[recs] Test entity component system API.\n";
    std::cout << "[recs] Starting.\n";

    test_entity_lifecycle();
    test_add_get_component();
    test_query_iteration();
    test_archetype_migration();

    std::cout << "[recs] Done.\n";
    std::cout << "[recs] All ECS tests passed.\n";
    return 0;
}
