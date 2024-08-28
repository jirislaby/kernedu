sources = [
  'irq.c',
  'irq_entry.s',
  'output.c',
  'setup.c',
  'string.c',
]
common_lib = static_library('common', sources)

common_lib_dep = declare_dependency(link_with : common_lib,
  include_directories : '.')
