import bpy
from collections import Counter
print("LASTLIGHT_BLENDER_FILE",bpy.data.filepath)
objects=list(bpy.data.objects)
print("LASTLIGHT_BLENDER_TYPES",dict(Counter(o.type for o in objects)))
for o in objects:
    if o.type=='ARMATURE':
        print("LASTLIGHT_ARMATURE",o.name,"bones",len(o.data.bones))
    if o.type=='MESH':
        mods=[(m.type,m.object.name if getattr(m,'object',None) else "") for m in o.modifiers]
        print("LASTLIGHT_MESH",o.name,"verts",len(o.data.vertices),"parent",o.parent.name if o.parent else None,"mods",mods,"groups",len(o.vertex_groups))
for a in bpy.data.actions:
    print("LASTLIGHT_ACTION",a.name,"range",tuple(a.frame_range))
