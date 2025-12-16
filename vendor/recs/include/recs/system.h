#pragma once

class World;

class System {
public:
    virtual ~System() = default;
    virtual void run(World& world, float delta_time) = 0;
};
