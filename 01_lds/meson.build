local_linker_script_file = meson.current_source_dir() + '/intro.lds'

executable('intro', [
  'intro.c',
  ],
  link_depends: local_linker_script_file,
  link_args: [ '-T', local_linker_script_file ],
  )
