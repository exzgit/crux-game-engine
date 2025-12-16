#pragma once

#include "component_registry.h"
#include <type_traits>

template<typename T>
struct is_component {
    static constexpr bool value =
        std::is_object_v<T> &&
        !std::is_reference_v<T> &&
        !std::is_pointer_v<T>;
};

template<typename T>
inline constexpr bool is_component_v = is_component<T>::value;

template<typename T>
ComponentTypeID component_type() {
    static_assert(is_component_v<T>, "T must be a valid component type");
    return ComponentRegistry::instance().register_component<T>();
}

template<typename T>
constexpr ComponentTypeID component_type_cached() {
    static_assert(is_component_v<T>, "T must be a valid component type");
    static ComponentTypeID id = ComponentRegistry::instance().register_component<T>();
    return id;
}
