import unreal,os
unreal.log("LASTLIGHT_MAP_MIGRATION_START")
sub=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
for target in ["/Game/Maps/Explore","/Game/Maps/Arena"]:
    if unreal.EditorAssetLibrary.does_asset_exist(target):
        unreal.log("LASTLIGHT_MAP_EXISTS "+target)
        continue
    result=sub.new_level(target)
    unreal.log("LASTLIGHT_NEW_LEVEL "+target+" result="+str(result))
    if not unreal.EditorAssetLibrary.does_asset_exist(target):
        raise RuntimeError("map not created: "+target)
    if not unreal.EditorAssetLibrary.save_asset(target):
        raise RuntimeError("cannot save map: "+target)
unreal.log("LASTLIGHT_MAP_MIGRATION_PASS")
