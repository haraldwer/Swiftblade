#m7 #rendering 
I've decided to lean into MSAA for this renderer
To do this, I'll render the frame using MSAA
I'll also render a slim GBuffer in the same pass (for SSAO, SSR etc)
The frame and gbuffer has to be resolved at some point

Options:
A: Specific resolve pass that converts the textures
B: Use LoadTexture with multisampled textures to resolve it on each read

SSAO and SSR does multiple samples and probably uses blending, both for frame and gbuffer
That means I need to resolve first


