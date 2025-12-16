project('CruxGameEngine', ['cpp', 'c'],
  version: '0.1',
  default_options: ['warning_level=2', 'cpp_std=c++20']
)

x11_dep = dependency('x11')
glfw_dep = dependency('glfw3')
glm_dep = dependency('glm')

glad_src = 'vendor/glad/src/glad.c'
glad_inc = include_directories('vendor/glad/include')

recs_inc = include_directories('vendor/recs/include')

imgui_inc = include_directories('vendor/imgui', 'vendor/imgui/backends')

libimgui = static_library('__imgui__',
  [
    'vendor/imgui/imgui.cpp',
    'vendor/imgui/imgui_draw.cpp',
    'vendor/imgui/imgui_tables.cpp',
    'vendor/imgui/imgui_widgets.cpp',
    'vendor/imgui/backends/imgui_impl_glfw.cpp',
    'vendor/imgui/backends/imgui_impl_opengl3.cpp'
  ],
  include_directories: imgui_inc,
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

libengine = static_library('__crux_engine__',
  [
    glad_src,
    'engine/core/error.cpp',
    'engine/core/input.cpp',
    'engine/core/time.cpp',
    'engine/engine.cpp'
  ],
  include_directories: [glad_inc, recs_inc],
  dependencies: [glfw_dep, glm_dep]
)

libeditor = static_library('__crux_editor__',
  [
    'editor/main.cpp',
    'editor/editor.cpp'
  ],
  include_directories: [glad_inc, imgui_inc, recs_inc],
  dependencies: [glfw_dep, glm_dep]
)


# executable('CruxEngine',
#   [
#     'engine/main.cpp',
#     'engine/core/error.cpp',
#     'engine/core/input.cpp',
#     'engine/core/time.cpp',
#     'engine/engine.cpp'
#   ],
#   include_directories: [glad_inc, recs_inc],
#   link_with: [libengine, librecs]
# )

executable('CruxEditor',
  'editor/main.cpp',
  include_directories: [glad_inc, recs_inc, imgui_inc],
  link_with: [libengine, libeditor, librecs, libimgui],
  dependencies: [x11_dep]
)
