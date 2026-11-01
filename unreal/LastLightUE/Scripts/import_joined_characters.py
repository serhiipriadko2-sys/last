import unreal,os
ROOT=r"E:\LastLightUnreal\LastLightUE\Migration\JoinedExports"
DEST="/Game/Characters/Joined"
AT=unreal.AssetToolsHelpers.get_asset_tools()
unreal.EditorAssetLibrary.make_directory(DEST)
ok=0
failed=[]
for name in ["hero","knight","wolf","wasp","golem","wraith"]:
    path=os.path.join(ROOT,name+".glb")
    if not os.path.isfile(path):
        failed.append(name+": missing source");continue
    job=unreal.AssetImportTask()
    job.set_editor_property("filename",path)
    job.set_editor_property("destination_path",DEST+"/"+name)
    job.set_editor_property("automated",True)
    job.set_editor_property("replace_existing",False)
    job.set_editor_property("save",True)
    try:
        AT.import_asset_tasks([job])
        objects=list(job.get_editor_property("imported_object_paths"))
        unreal.log("LASTLIGHT_CHARACTER_IMPORT "+name+" "+repr(objects))
        if objects:ok+=1
        else:failed.append(name+": empty import")
    except Exception as exc:
        failed.append(name+": "+str(exc))
assets=unreal.EditorAssetLibrary.list_assets(DEST,recursive=True,include_folder=False)
unreal.log("LASTLIGHT_ASSETS_FOUND "+str(len(assets)))
for a in assets[:70]:
    try:
        obj=unreal.EditorAssetLibrary.load_asset(a)
        unreal.log("LASTLIGHT_ASSET "+a+" class="+str(obj.get_class().get_name()))
    except Exception as exc:unreal.log_warning("asset class error "+str(exc))
unreal.log("LASTLIGHT_JOINED_IMPORT_RESULT imported="+str(ok)+" failed="+str(failed))
