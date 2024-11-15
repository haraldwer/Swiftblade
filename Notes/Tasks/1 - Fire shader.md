Current render setup: 

Entries -> GBuffer
GBuffer -> SSAO
GBuffer + AO -> FrameTarget (ds)
GBuffer + AO + Frame -> Quantize
Quantize -> FXAA


Fire requires: 
Velocity
Lifetime
What happens when fire goes outside the screen?