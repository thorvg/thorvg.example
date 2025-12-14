project('thorvg.example',
  'cpp',
  version: '1.0',
  default_options: ['buildtype=debugoptimized', 'cpp_std=c++14', 'optimization=3'],
  license : 'MIT')

add_project_arguments('-DEXAMPLE_DIR="@0@/res"'.format(meson.current_source_dir()), language : 'cpp')

subdir('src')