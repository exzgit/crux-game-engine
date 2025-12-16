project('CruxGameEngine', ['cpp', 'c'],
  version: '0.1',
  default_options: ['warning_level=2', 'cpp_std=c++20']
)

glfw_dep = dependency('glfw3')
glm_dep = dependency('glm')

glad_src = 'vendor/glad/src/glad.c'
glad_inc = include_directories('vendor/glad/include')

recs_inc = include_directories('vendor/recs/include')

libengine = static_library('__crux_engine__',
  [
    glad_src
  ],
  include_directories: glad_inc,
  dependencies: [glfw_dep, glm_dep]
)

libeditor = static_library('__crux_editor__',
  [
    'editor/main.cpp',
  ],
  include_directories: glad_inc,
  dependencies: [glfw_dep, glm_dep]
)

librecs = static_library('__crux_ecs__',
  [
    'vendor/recs/src/archetype.cpp',
    'vendor/recs/src/archetype_manager.cpp',
    'vendor/recs/src/chunk.cpp',
    'vendor/recs/src/component_registry.cpp',
    'vendor/recs/src/entity_manager.cpp',
    'vendor/recs/src/query.cpp',
    'vendor/recs/src/world.cpp'
  ],
  include_directories: recs_inc,
  dependencies: [glfw_dep, glm_dep]
)

executable('CruxEngine',
  [
    'engine/main.cpp',
    'engine/core/error.cpp',
    'engine/core/input.cpp',
    'engine/core/time.cpp'
  ],
  include_directories: [glad_inc, recs_inc],
  link_with: [libengine, librecs] 
)

executable('CruxEditor',
  'editor/main.cpp',
  include_directories: [glad_inc, recs_inc],
  link_with: [libengine, libeditor, librecs]
)
