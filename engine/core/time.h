#pragma once

#include <GLFW/glfw3.h>

class Time {
public:
    static float delta_time;
    static float last_frame;

    static void update_deltatime() {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
    }
};

