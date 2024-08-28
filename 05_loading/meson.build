os = executable('os', [
  'loader.s',
  'os.c',
  ],
  link_depends: linker_script_file,
  dependencies: [ common_lib_dep, linker_script ],
  )

custom_target(
  build_by_default: true,
  input: os,
  output: 'os.bin',
  command: [ objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@' ],
  )
