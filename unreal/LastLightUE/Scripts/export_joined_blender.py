import bpy, pathlib, sys
from collections import Counter
stem=pathlib.Path(bpy.data.filepath).stem
out=pathlib.Path("E:/LastLightUnreal/LastLightUE/Migration/JoinedExports")
out.mkdir(parents=True,exist_ok=True)
rigs=[o for o in bpy.data.objects if o.type=="ARMATURE"]
meshes=[o for o in bpy.data.objects if o.type=="MESH"]
assert len(rigs)==1 and len(meshes)>2,(stem,len(rigs),len(meshes))
rig=rigs[0]
print("LL_JOIN_BEGIN",stem,len(meshes),"bones",len(rig.data.bones),
      "verts",sum(len(x.data.vertices) for x in meshes))
# Bake no modifiers: join mesh parts and preserve armature/vertex groups.
# All source meshes have the same armature object and parent, so no retargeting.
bpy.ops.object.mode_set(mode='OBJECT') if bpy.context.object and bpy.context.object.mode!='OBJECT' else None
bpy.ops.object.select_all(action='DESELECT')
for m in meshes:m.select_set(True)
active=meshes[0]
bpy.context.view_layer.objects.active=active
bpy.ops.object.join()
active.name="SKM_"+stem
for m in list(active.modifiers):
    if m.type=="ARMATURE":m.object=rig
# Ensure only one armature modifier remains.
armmods=[m for m in active.modifiers if m.type=="ARMATURE"]
for m in armmods[1:]:active.modifiers.remove(m)
if not armmods:
    mod=active.modifiers.new("Armature","ARMATURE")
    mod.object=rig
print("LL_JOIN_RESULT",stem,"mesh_objects",len([o for o in bpy.data.objects if o.type=="MESH"]),
      "verts",len(active.data.vertices),"groups",len(active.vertex_groups),
      "armmods",len([m for m in active.modifiers if m.type=="ARMATURE"]))
# Do not touch original blend: export only.
bpy.ops.object.select_all(action="DESELECT")
rig.select_set(True)
active.select_set(True)
bpy.context.view_layer.objects.active=rig
filepath=out/(stem+".glb")
bpy.ops.export_scene.gltf(filepath=str(filepath),export_format="GLB",
    use_selection=True,export_animations=True,export_skins=True,
    export_yup=True)
print("LL_EXPORT_RESULT",stem,str(filepath),"bytes",filepath.stat().st_size)
