#archive 
Currently: 
Every object renders to gbuffer using its surface material
The deferred renderer uses a default deferred shader

Desired: 
Every object renders to gbuffer using a surface material, but also specifies a deferred shader
The deferred renderer splits objects based on deferred shader and render this

