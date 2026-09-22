cc = meson.get_compiler('cpp')
thorvg_dep = dependency('thorvg-1', required: true)
toolkit_dep = dependency('thorvg-toolkit', required: true)
examples_dep = [thorvg_dep, toolkit_dep]

source_file = [
    'Accessor.cpp',
    'Animation.cpp',
    'Blending.cpp',
    'BoundingBox.cpp',
    'Clipping.cpp',
    'ConicGradient.cpp',
    'CustomTransform.cpp',
    'DataLoad.cpp',
    'DirectUpdate.cpp',
    'Duplicate.cpp',
    'EffectDropShadow.cpp',
    'GifAnimation.cpp',
    'GifSaver.cpp',
    'SceneEffects.cpp',
    'FillRule.cpp',
    'FillSpread.cpp',
    'GradientMasking.cpp',
    'GradientStroke.cpp',
    'GradientTransform.cpp',
    'Intersects.cpp',
    'LinearGradient.cpp',
    'Masking.cpp',
    'MaskingMethods.cpp',
    'Opacity.cpp',
    'Path.cpp',
    'Picture.cpp',
    'PictureData.cpp',
    'PictureJpg.cpp',
    'PicturePng.cpp',
    'PictureRaw.cpp',
    'PictureTransform.cpp',
    'PictureSvg.cpp',
    'PictureWebp.cpp',
    'RadialGradient.cpp',
    'Retaining.cpp',
    'Scene.cpp',
    'SceneBlending.cpp',
    'SceneTransform.cpp',
    'Shapes.cpp',
    'SmartRender.cpp',
    'Stroke.cpp',
    'StrokeLine.cpp',
    'StrokeMiterlimit.cpp',
    'Svg.cpp',
    'TextOtf.cpp',
    'TextTtf.cpp',
    'TextEffects.cpp',
    'TextLayout.cpp',
    'TextLineWrap.cpp',
    'TextMetrics.cpp',
    'Transform.cpp',
    'TrimPath.cpp',
    'Viewport.cpp'
]

thorvg_inc = include_directories(join_paths(thorvg_dep.get_variable(pkgconfig: 'includedir'), 'thorvg-1'))

# Lottie Examples
lottie_example = '''
    #include <thorvg_lottie.h>
    int main(int argc, char **argv) {
        return tvg::LottieAnimation::gen() ? 0 : 1;
    }
'''

if cc.links(lottie_example, dependencies : thorvg_dep, include_directories : thorvg_inc)
    source_file += [
        'Lottie.cpp',
        'LottieAssetResolver.cpp',
        'LottieAudio.cpp',
        'LottieExpressions.cpp',
        'LottieInteraction.cpp',
        'LottieSlot.cpp',
        'LottieTweening.cpp'
    ]
    message('ThorVG Lottie found. Enabled Lottie examples.')
else
    message('ThorVG Lottie not found. Skipping Lottie examples.')
endif

# Media Examples
media_example = '''
    #include <thorvg_media.h>
    int main(int argc, char **argv) {
        return tvg::Video::gen() ? 0 : 1;
    }
'''

if cc.links(media_example, dependencies : thorvg_dep, include_directories : thorvg_inc)
    source_file += ['Video.cpp']
    message('ThorVG Media found. Enabled Video example.')
else
    message('ThorVG Media not found. Skipping Video example.')
endif

foreach current_file : source_file
    name = current_file.split('.')[0]
    executable(name, current_file,
        dependencies: examples_dep)
endforeach

if cc.get_id() == 'msvc'
    configure_file(input: 'all.bat', output: 'all.bat', copy: true)
else
    configure_file(input: 'all.sh', output: 'all.sh', copy: true)
endif
