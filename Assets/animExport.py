import bpy
import json
import os

# =============================
# USER CONFIGURATION
# =============================

# Folder where JSON files will be exported.
EXPORT_DIR = bpy.path.abspath("//animation_exports")

os.makedirs(EXPORT_DIR, exist_ok=True)

# =============================
# MAIN EXPORT LOGIC
# =============================

def get_actions_for_selected_objects():
    """Collect unique actions used by selected objects."""
    actions = {}
    for obj in bpy.context.selected_objects:
        if obj.animation_data and obj.animation_data.action:
            actions.setdefault(obj.animation_data.action, []).append(obj)
        if obj.animation_data and obj.animation_data.nla_tracks:
            for track in obj.animation_data.nla_tracks:
                for strip in track.strips:
                    if strip.action:
                        actions.setdefault(strip.action, []).append(obj)
    return actions


def export_action_to_json(action, objects, export_path):
    """Export one action’s FCurves, grouped by object name."""
    
    frame_start, frame_end = action.frame_range
    fps = bpy.context.scene.render.fps
    duration_frames = frame_end - frame_start
    duration_seconds = duration_frames / fps
    
    export_data = {
        "action_name": action.name,
        "frame_range": [frame_start, frame_end],
        "duration_frames": duration_frames,
        "duration_seconds": duration_seconds,
        "fps": fps,
        "objects": {},
        "markers": []
    }
    
    # Collect FCurves per object
    for obj in objects:
        obj_curves = []
        for fcurve in action.fcurves:
            # Filter by object if the FCurve belongs to it
            # For armatures, data_path might contain bone paths like pose.bones["Bone"].location
            if fcurve.group and fcurve.group.name == obj.name:
                include = True
            elif fcurve.data_path.startswith("pose.bones"):
                include = obj.type == 'ARMATURE'
            elif fcurve.data_path.startswith("location") or fcurve.data_path.startswith("rotation") or fcurve.data_path.startswith("scale"):
                include = True
            else:
                include = False

            if not include:
                continue

            curve_info = {
                "data_path": fcurve.data_path,
                "array_index": fcurve.array_index,
                "keyframes": []
            }

            for kp in fcurve.keyframe_points:
                if kp.interpolation == "CONSTANT":
                    interp = 2
                elif kp.interpolation == "BEZIER":
                    interp = 1
                else:
                    interp = 0
                
                key_info = {
                    "frame": float(kp.co.x),
                    "value": float(kp.co.y),
                    "interpolation": interp,
                    "handle_left": [float(kp.handle_left.x), float(kp.handle_left.y)],
                    "handle_right": [float(kp.handle_right.x), float(kp.handle_right.y)],
                }
                curve_info["keyframes"].append(key_info)

            obj_curves.append(curve_info)

        export_data["objects"][obj.name] = {"fcurves": obj_curves}

    # Export timeline markers (events)
    for marker in bpy.context.scene.timeline_markers:
        export_data["markers"].append({
            "name": marker.name,
            "frame": marker.frame
        })

    # Write JSON file
    with open(export_path, "w", encoding="utf-8") as f:
        json.dump(export_data, f, indent=2)
    print(f"✅ Exported: {export_path}")


def main():
    actions_map = get_actions_for_selected_objects()

    if not actions_map:
        print("⚠️ No animations found on selected objects.")
        return

    print(f"Exporting {len(actions_map)} actions to {EXPORT_DIR} ...")

    for action, objs in actions_map.items():
        filename = f"{action.name}.json"
        export_path = os.path.join(EXPORT_DIR, filename)
        export_action_to_json(action, objs, export_path)

    print("🎉 Export complete.")


if __name__ == "__main__":
    main()
