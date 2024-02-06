import bpy
import os

# Get the path where the blend file is located
basedir = bpy.path.abspath('//')

# loop through all the objects in the scene
scene = bpy.context.scene
for ob in bpy.context.selected_objects:
    # Select each object
    ob.select_set(True)

    # Make sure that we only export meshes
    if ob.type == 'MESH':
        # Export the currently selected object to its own file based on its name
        bpy.ops.wm.obj_export(
            filepath=os.path.join(basedir, 'M_' + ob.name + '.obj'),
            export_selected_objects=True,
            )

    # Deselect the object and move on to another if any more are left
    ob.select_set(False)