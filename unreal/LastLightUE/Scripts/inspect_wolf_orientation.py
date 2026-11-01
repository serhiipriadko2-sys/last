import bpy
from mathutils import Vector
print("WOLF_DIRECTION_SCENE", bpy.data.filepath)
for o in bpy.data.objects:
 if o.type=="MESH":
  print("WOLF_PART",o.name,"world_loc",tuple(round(x,1) for x in o.matrix_world.translation),"dims",tuple(round(x,1) for x in o.dimensions))
 if o.type=="ARMATURE":
  print("WOLF_ARMATURE",o.name,"rotation",tuple(round(x,2) for x in o.rotation_euler))
for b in bpy.data.objects:
 if b.type=="ARMATURE":
  for bone in b.data.bones:
   if any(t in bone.name.lower() for t in ('head','nose','muzzle','neck')):
    print("WOLF_BONE",bone.name,"head",tuple(round(v,1) for v in bone.head),"tail",tuple(round(v,1) for v in bone.tail))
