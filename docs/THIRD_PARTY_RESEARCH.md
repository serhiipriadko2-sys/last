# Assessment of the ten suggested open-source game projects

Research updated 2026-11-01. **No foreign library source, textures, animations or engines were integrated into the build.** The list is a design reference, not an instruction to install everything. Copying every project would increase maintenance and dependency risk without helping the inverse-progression prototype.

| Repository | Potential contribution | Decision for future Unreal build |
| --- | --- | --- |
| [Jolt Physics](https://github.com/jrouwe/JoltPhysics) | C++ rigid-body simulation | Defer. Unreal already provides Chaos Physics; adopting a second physics stack requires benchmark and integration proof. |
| [Terrain3D](https://github.com/TokisanGames/Terrain3D) | High-quality editable terrain | Reject for Unreal directly; this is a Godot 4 GDExtension. Apply heightmap/LOD ideas, not its binaries. |
| [Material Maker](https://github.com/RodZill4/material-maker) | Procedural material authoring | Optional offline authoring workflow; review licensing/texture export before use. |
| [Effekseer](https://github.com/effekseer/Effekseer) | Magical particle/VFX authoring | Defer. Start with UE Niagara and compare quality/cost against Effekseer export/integration. |
| [Voxel Tools for Godot](https://github.com/Zylann/godot_voxel) | Procedural caves and editable worlds | Reject direct Unreal dependency; Godot module/extension, not UE plugin. Apply chunking/LOD concepts if destructibility becomes a proven requirement. |
| [Recast Navigation](https://github.com/recastnavigation/recastnavigation) | Navmesh and pathfinding | Explore UE navmesh before a standalone fork; NPC navigation is not currently a performance bottleneck. |
| [ozz-animation](https://github.com/guillaumeblanc/ozz-animation) | Animation sampling/blending | Defer; Unreal animation blueprints/control rig/retargeting better fit the native pipeline. |
| [The Forge](https://github.com/ConfettiFX/The-Forge) | Cross-platform rendering framework | Exclude; replacing Unreal's renderer violates the scope of a gameplay prototype. |
| [meshoptimizer](https://github.com/zeux/meshoptimizer) | Mesh simplification and packing | Consider offline asset import optimization when profiling reveals a GPU/mesh bottleneck. Check license/attribution if used. |
| [FastNoise2](https://github.com/Auburn/FastNoise2) | Fast procedural noise | Optional future procedural terrain seed. Current arena is intentionally hand-constrained. |

Upstream references consulted: Epic [Chaos Physics](https://dev.epicgames.com/documentation/unreal-engine/physics-in-unreal-engine), Godot [Terrain3D installation](https://github.com/TokisanGames/Terrain3D/blob/main/doc/docs/installation.md), [Voxel Tools overview](https://github.com/Zylann/godot_voxel), and [meshoptimizer README](https://github.com/zeux/meshoptimizer). Each future integration requires specific version pin, compatibility test, license review, performance measurement and rollback path.
