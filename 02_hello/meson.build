local_linker_script_file = meson.current_source_dir() + '/os.lds'

os = executable('os', [
  'loader.s',
  ],
  link_depends: local_linker_script_file,
  link_args: [ '-T', local_linker_script_file ],
  dependencies: [ build_os_dep ],
  )

custom_target(
  build_by_default: true,
  input: os,
  output: 'os.bin',
  command: [ objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@' ],
  )
