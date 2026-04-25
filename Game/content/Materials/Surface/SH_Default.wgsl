
@group(0) @binding(0) var<uniform> ViewProj: mat4x4<f32>;

@group(1) @binding(0) var<storage, read> instances : array<mat4x4<f32>>;
@group(1) @binding(1) var<storage, read> meshes  : array<mat4x4<f32>>;  // Single matrix per mesh

struct VSOut 
{
    @builtin(position) position: vec4f,
    @location(0) normal: vec3f,
    @location(1) uv: vec2f,
}; 

struct FSOut
{
    @location(0) frame: vec4f,
    @location(1) normal: vec4f,
};

@vertex
fn vs_main(
    @builtin(instance_index) instanceIdx : u32,
    @location(0) position: vec3f, 
    @location(1) normal: vec3f, 
    @location(2) uv: vec2f
    ) -> VSOut 
{
    let modelIdx = instanceIdx / arrayLength(&meshes);
    let meshIdx  = instanceIdx % arrayLength(&meshes);
    let world = instances[modelIdx] * meshes[meshIdx];
    

    var out: VSOut;
    out.position = (ViewProj * world) * vec4f(position, 1.0);
    out.normal = normal;
    out.uv = uv;
    return out;
}


@fragment
fn fs_main(
    @location(0) normal: vec3f,
    @location(1) uv: vec2f
    ) -> FSOut
{
    var out: FSOut;
    out.frame = vec4f(abs(normal), 1.0);
    out.normal = vec4f(normal, 1.0);
    return out;
}