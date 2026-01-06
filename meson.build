project('thorvg.example',
  'cpp',
  version: '1.0',
  default_options: ['buildtype=debugoptimized', 'cpp_std=c++14', 'optimization=3'],
  license : 'MIT')

example_dir = meson.current_source_dir()
if host_machine.system() == 'windows'
  example_dir = example_dir.replace('\\', '/')
endif
add_project_arguments('-DEXAMPLE_DIR="@0@/res"'.format(example_dir), language : 'cpp')

subdir('src')