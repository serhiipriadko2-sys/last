# LAST LIGHT v0.6 — аудит 10 репозиториев и план интеграции

Дата: 2026-11-01. Windows x64, Godot 4.7.2. [FACT] Перед улучшениями обнаружено рассогласование фронта GLB-риггинга и Godot 3D. Эти исправления важнее установки тяжёлых дополнений.

## Что из десяти действительно внедрено

| Исходный проект | Решение в LAST LIGHT v0.6 | Статус |
|---|---|---|
| Jolt Physics (jrouwe/JoltPhysics) | В project.godot явно выбран встроенный Jolt Physics. Проверены коллизии, прыжки и пять обходов. | IMPLEMENTED, встроенный backend |
| Terrain3D (TokisanGames/Terrain3D) | 10 процедурных cliff-мешей вне игровой дороги, биомные цвета. Полный Terrain3D GDExtension не требуется линейному миру. | NATIVE ALTERNATIVE, Terrain3D NOT INSTALLED |
| Material Maker (RodZill4/material-maker) | Процедурные материалы с roughness/metallic и вершинной окраской. | NATIVE ALTERNATIVE, APP NOT INSTALLED |
| Effekseer (effekseer/EffekseerForGodot4) | CPUParticles3D, анимированные ударные кольца, динамическая камера. Внешний плагин не установлен. | NATIVE ALTERNATIVE |
| Voxel Tools (Zylann/godot_voxel) | Отложено: изменение мира в игре, туннели и блоки требуют другого дизайна, коллизий, сохранений. | NOT INSTALLED |
| Recast (recastnavigation/recastnavigation) | Выявлено, что Godot NavigationServer3D уже позволяет строить navmesh. При статичных врагах его внедрение пока неоправданно. | RESEARCHED, NOT INSTALLED |
| ozz-animation (guillaumeblanc/ozz-animation) | Сохранены 6 Blender-ригов/7 клипов и смешивание Godot AnimationPlayer; исправлен визуальный front adapter. | NATIVE ALTERNATIVE, C++ OZZ NOT INSTALLED |
| The Forge (ConfettiFX/The-Forge) | Не заменяем Godot низкоуровневым графическим framework. | REJECTED FOR CURRENT ARCHITECTURE |
| meshoptimizer (zeux/meshoptimizer) | MultiMeshInstance3D: 380 камней/кристаллов пятью группами биомов, без сотен draw calls. gltfpack не запускался. | NATIVE BATCHING, LIB NOT INSTALLED |
| FastNoise2 (Auburn/FastNoise2) | Встроенный FastNoiseLite FBM с постоянным seed создаёт живописные скалы. | NATIVE ALTERNATIVE, C++ FASTNOISE2 NOT INSTALLED |

Это НЕ утверждение о том, что десять библиотек были установлены. Прямое включение десяти разных C++ расширений повышает риск несовместимости, хрупкой упаковки и неожиданного падения Windows-сборки. Обходной путь выбран осознанно, а не как замена нужного функционала.

## Исправления ориентации
- Blender/glTF персонажи имеют визуальный перед по +Z, а Godot `Node3D.look_at` направляет локальную -Z. `scripts/rig_actor.gd` вращает только импортированный визуальный GLB на 180°.
- Процедурные персонажи, сделанные лицом вдоль +X, используют `VisualFacingPivot` (+90°). Физический `CharacterBody3D` никогда не поворачивается ради этой компенсации.
- Боевая арена вызывает `look_at` взаимно для двух бойцов; `facing_alignment` тестирует расстояние до цели и dot-product.
- Камера по умолчанию ориентирована вдоль основной дороги +X. Теперь **W = вперёд по главному маршруту**, R возвращает этот же ракурс. WASD относителен камере, диагонали нормализованы.

## Современный UI
- Вместо длинных технических сводок: слева уровень/здоровье/основные ресурсы, сверху цель/HP врага, справа контекстные действия.
- Тайминг Q/E и Space отображается крупной шкалой по центру внизу, золотая метка соответствует конкретному удару (63/72/81% либо 67%).
- Два основных действия в бою показаны явно; остальные способности фильтруются по типу и листаются по пять.
- 100 навыков получили русские поля `name_ru`, не изменяя IDs, урон, формулы и сохранения. Сила навыка и стихийная слабость противника видны рядом с названием.
- В режиме исследования основные кнопки видны сразу, остальные под `ИНВЕНТАРЬ, КУЗНИЦА И ДРУГОЕ`. H раскрывает подсказки управления.

## Проверка и реальные ограничения
- `tests/orientation_ui_test.gd`: 12 направлений персонажа, 5 арен, UI, Jolt, шумовые меши и батчинг.
- `tests/localization_test.gd`: 100 уникальных русских названий, старые skill IDs и отображение в интерфейсе.
- Остальные 12 проверок предыдущих выпусков остаются обязательными: физические проходы, 30 мобов, 9 мини-боссов, 7 анимаций каждого рига, Space/Q/E, деградация 100→0, финальное зеркало.
- Отдельный рендер 1600x900 и 1366x768 — визуальное подтверждение, не человеческий плейтест.

НЕ РЕАЛИЗОВАНО: 30 студийных персонажей, полнофункциональный navmesh-AI, разрушаемые воксели, The Forge backend, киноанимация уровня AAA, реальный пользовательский тест новичков и коммерческая готовность. Подключение библиотеки не равно повышению качества игры.

## Источники
Jolt: https://docs.godotengine.org/en/4.7/tutorials/physics/using_jolt_physics.html
Terrain3D: https://github.com/TokisanGames/Terrain3D/releases
Material Maker: https://github.com/RodZill4/material-maker
Effekseer: https://github.com/effekseer/EffekseerForGodot4/releases
Voxel: https://github.com/Zylann/godot_voxel
Recast: https://github.com/recastnavigation/recastnavigation
Ozz: https://github.com/guillaumeblanc/ozz-animation
The Forge: https://github.com/ConfettiFX/The-Forge
meshoptimizer: https://github.com/zeux/meshoptimizer/blob/master/gltf/README.md
FastNoise2: https://github.com/Auburn/FastNoise2
Godot front axes: https://docs.godotengine.org/en/4.5/classes/class_node3d.html

∆DΩΛ: ∆ улучшены направления/камера/UI/сцены; D исходный код, тесты, кадры, архивы; Ω высокая техническая по тестам, ограниченная художественная; Λ пересмотр после живого плейтеста и сравнения FPS.
