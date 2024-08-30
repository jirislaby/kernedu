project('kernedu', 'c',
  version: '1',
  default_options: [
    'optimization=s',
    'warning_level=2',
  ])

build_os_dep = declare_dependency(
  compile_args: [
    '-ggdb',
    '-m32',
    '-ffreestanding',
    '-nostdinc',
    '-fmessage-length=0',
    '-mno-red-zone',
    '-fno-stack-protector',
  ],
  link_args: [
    '-m32',
    '-nostdlib',
    '-z', 'noexecstack',
  ])

linker_script_file = meson.current_source_dir() + '/05_loading/os.lds'
linker_script = declare_dependency(
  link_args: [ '-T', linker_script_file ],
  )

objcopy = find_program('objcopy')

subdir('common')

subdir('01_lds')
subdir('02_hello')
subdir('03_c')
subdir('04_protected')
subdir('05_loading')
subdir('06_keyboard')
subdir('07_interrupt')
subdir('09_timer')
subdir('10_cpuid')
subdir('11_pci')
