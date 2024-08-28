project('kernedu', 'c',
  version: '1',
  default_options: [
    'optimization=s',
    'warning_level=2',
  ])

add_project_arguments([
    '-ggdb',
    '-m32',
    '-ffreestanding',
    '-nostdinc',
    '-fmessage-length=0',
    '-mno-red-zone',
    '-fno-stack-protector',
  ], language : 'c')

add_project_link_arguments([
    '-m32',
    '-nostdlib',
    '-z', 'noexecstack',
  ], language : 'c')

linker_script_file = meson.current_source_dir() + '/05_loading/os.lds'
linker_script = declare_dependency(
  link_args: [ '-T', linker_script_file ],
  )

objcopy = find_program('objcopy')

subdir('common')

subdir('05_loading')
subdir('06_keyboard')
subdir('07_interrupt')
subdir('09_timer')
subdir('10_cpuid')
subdir('11_pci')
