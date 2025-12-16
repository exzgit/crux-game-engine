#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>
#include <algorithm>
#include <string>

struct Entity {
  std::uint32_t index;
  std::uint32_t generation;

  constexpr Entity(std::uint32_t i = invalid_index(), std::uint32_t g = 0u) noexcept
      : index(i), generation(g) {}

  bool operator==(const Entity& o) const noexcept {
    return o.index == index && o.generation == generation;
  }

  bool operator!=(const Entity& o) const noexcept { return !(*this == o); }

  static constexpr std::uint32_t invalid_index() noexcept {
    return std::numeric_limits<std::uint32_t>::max();
  }

  static constexpr Entity invalid() noexcept { return Entity{invalid_index(), 0u}; }
};

struct Family {
  Entity parent;
  std::vector<Entity> children;

  Family() : parent(Entity::invalid()) {}

  void add_child(Entity child) {
    if (child == Entity::invalid()) return;
    if (std::find(children.begin(), children.end(), child) == children.end()) {
      children.push_back(child);
    }
  }

  void remove_child(Entity child) {
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
  }
};

struct Identity {
  std::string name;
  std::string tag;
  std::string layer_class;

  Identity() : name(""), tag("Default"), layer_class("Default") {}
  Identity(std::string n, std::string t = "Default", std::string layer = "Default")
      : name(std::move(n)), tag(std::move(t)), layer_class(std::move(layer)) {}
};