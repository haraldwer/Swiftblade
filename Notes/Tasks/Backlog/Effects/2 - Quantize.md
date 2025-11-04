Some kind of quantization
Currently considering two options:
### 1. Low res PBR rendering and smart upsample
Render PBR to a really small target
Later scale up but using GBuffer info, effectively rounding to closest match for each pixel
How to handle  cases where no match exists? Blend a base color? Unclear...
### 2. GBuffer-gather world-space coordinate rounding
Also a smart solution, however its hard to optimize the PBR workflow with this approach
We want to find a stylistic way to quantize that also lets us skip some work

