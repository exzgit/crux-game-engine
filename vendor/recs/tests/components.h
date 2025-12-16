#pragma once

#include <cstdint>

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct Health {
    std::int32_t value;
};
