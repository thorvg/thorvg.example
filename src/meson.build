cc = meson.get_compiler('cpp')
thorvg_dep = dependency('thorvg-1', required: true)
examples_dep = [thorvg_dep, dependency('sdl2', required: true)]
compiler_flags = ['-DSDL_MAIN_HANDLED']

# GL or GLES
gl_dep = dependency('gl', required: false)
gles_dep = dependency('glesv2', required: false)

if gl_dep.found()
    examples_dep += gl_dep
elif gles_dep.found()
    examples_dep += gles_dep
    compiler_flags += ['-DTVGEXAMPLE_GLES_SUPPORTED']
else
    message('Neither OpenGL nor OpenGL ES was found. Skipping GL examples.')
endif

# WebGPU
wgpu_dep = dependency('wgpu_native', required: false)

if wgpu_dep.found()
    examples_dep += wgpu_dep
    compiler_flags += ['-DTVGEXAMPLE_WGPU_SUPPORTED']
    if host_machine.system() == 'darwin'
        add_languages('objcpp', native: false)
        examples_dep += declare_dependency(link_args: ['-lobjc', '-framework', 'Cocoa', '-framework', 'CoreVideo', '-framework', 'IOKit', '-framework', 'QuartzCore'])
        compiler_flags += ['-x', 'objective-c++']
    endif
else
    message('wgpu_native not found, skipping WGPU examples.')
endif

source_file = [
    'Accessor.cpp',
    'Animation.cpp',
    'Blending.cpp',
    'BoundingBox.cpp',
    'Clipping.cpp',
    'CustomTransform.cpp',
    'DataLoad.cpp',
    'DirectUpdate.cpp',
    'Duplicate.cpp',
    'EffectDropShadow.cpp',
    'GifSaver.cpp',
    'SceneEffects.cpp',
    'FillRule.cpp',
    'FillSpread.cpp',
    'GradientMasking.cpp',
    'GradientStroke.cpp',
    'GradientTransform.cpp',
    'ImageRotation.cpp',
    'ImageScaling.cpp',
    'Intersects.cpp',
    'LinearGradient.cpp',
    'LumaMasking.cpp',
    'Masking.cpp',
    'MaskingMethods.cpp',
    'MultiCanvas.cpp',
    'Opacity.cpp',
    'Path.cpp',
    'Particles.cpp',
    'PictureJpg.cpp',
    'PicturePng.cpp',
    'PictureRaw.cpp',
    'PictureSvg.cpp',
    'PictureWebp.cpp',
    'RadialGradient.cpp',
    'Retaining.cpp',
    'Scene.cpp',
    'SceneBlending.cpp',
    'SceneTransform.cpp',
    'Shapes.cpp',
    'Stroke.cpp',
    'StrokeLine.cpp',
    'StrokeMiterlimit.cpp',
    'Svg.cpp',
    'Text.cpp',
    'TextLayout.cpp',
    'TextLineWrap.cpp',
    'Transform.cpp',
    'TrimPath.cpp',
    'Update.cpp',
    'Viewport.cpp'
]

thorvg_inc = include_directories(join_paths(thorvg_dep.get_variable(pkgconfig: 'includedir'), 'thorvg-1'))
thorvg_lottie = cc.has_header('thorvg_lottie.h', include_directories: thorvg_inc)

if thorvg_lottie
    source_file += [
        'Lottie.cpp',
        'LottieExpressions.cpp',
        'LottieExtension.cpp',
        'LottieInteraction.cpp',
        'LottieTweening.cpp'
    ]
else
    message('thorvg_lottie.h not found. Skipping Lottie examples.')
endif

foreach current_file : source_file
    name = current_file.split('.')[0]
    executable(name, current_file,
        dependencies: examples_dep,
        cpp_args : compiler_flags)
endforeach

if cc.get_id() == 'msvc'
    configure_file(input: 'all.bat', output: 'all.bat', copy: true)
else
    configure_file(input: 'all.sh', output: 'all.sh', copy: true)
endif