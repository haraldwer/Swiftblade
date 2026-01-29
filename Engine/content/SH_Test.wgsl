
//@group(0) @binding(0) var<uniform> model_view_matrix: mat4x4;

struct VSOut 
{
    @builtin(position) position: vec4f,
    @location(0) normal: vec3f,
    @location(1) uv: vec2f,
};

@vertex
fn vs_main(
    @location(0) position: vec3f, 
    @location(1) normal: vec3f, 
    @location(2) uv: vec2f
    ) -> VSOut 
{
    var out: VSOut;
    out.position = vec4f(position, 1.0);
    out.normal = normal;
    out.uv = uv;
    return out;
}


@fragment
fn fs_main(
    @location(0) normal: vec3f,
    @location(1) uv: vec2f
    ) -> @location(0) vec4f 
{
    return vec4f(0.0, 0.4, 1.0, 1.0);
}