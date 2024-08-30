sources = [
  'irq.c',
  'irq_entry.s',
  'output.c',
  'setup.c',
  'string.c',
]
common_lib = static_library('common', sources, dependencies: [ build_os_dep ])

common_include = declare_dependency(include_directories : '.')
common_lib_dep = declare_dependency(link_with: common_lib,
  dependencies: common_include)
