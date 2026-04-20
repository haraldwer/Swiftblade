
#include "Materials/Includes/SH_Fullscreen.wgsli"

@group(0) @binding(0) var TexFrame : texture_multisampled_2d<f32>;
@group(0) @binding(1) var TexNormals : texture_multisampled_2d<f32>;
// TODO: Sampler

@fragment
fn fs_main(@location(0) uv : vec2<f32>) -> @location(0) vec4<f32> 
{
    let texel = vec2<i32>(uv * vec2<f32>(textureDimensions(TexSource, 0)));
    return textureLoad(TexSource, texel, 0);
}
