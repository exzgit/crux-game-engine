project('RECS', ['cpp', 'c'],
  version: '0.1',
  default_options: ['warning_level=2', 'cpp_std=c++20']
)

recs_inc = include_directories('include/')

librecs = static_library('__recs__',
  [
    'src/archetype.cpp',
    'src/archetype_manager.cpp',
    'src/chunk.cpp',
    'src/component_registry.cpp',
    'src/entity_manager.cpp',
    'src/query.cpp',
    'src/world.cpp'
  ],
  include_directories: recs_inc
)

executable(
  'resc_test',
  [
    'tests/test_ecs.cpp'
  ],
  include_directories: recs_inc,
  link_with: librecs
)
