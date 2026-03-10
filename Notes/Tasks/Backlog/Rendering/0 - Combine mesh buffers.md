#m7 #rendering
Combine mesh buffers into one big buffer. When rendering a mesh, provide buffer offsets. 
Final buffer has to be read/write so that new meshes can be loaded. 
What about unloading meshes? - Skip for now
Should buffer be split into chunks? - Not right now