import bpy
import os

basedir = bpy.path.abspath('//')
scene = bpy.context.scene

objects = [o for o in bpy.context.selected_objects if o.type == 'MESH']
bpy.ops.object.select_all(action='DESELECT')

for ob in objects:
    ob.select_set(True)
    bpy.context.view_layer.objects.active = ob
    bpy.ops.wm.obj_export(
        filepath=os.path.join(basedir, f"M_{ob.name}.obj"),
        export_selected_objects=True,
        export_triangulated_mesh=True,
        export_normals=True,
        export_uv=True,
        export_materials=False,
    )
    ob.select_set(False)§