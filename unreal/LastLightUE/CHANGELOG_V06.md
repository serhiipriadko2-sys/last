# LAST LIGHT — журнал версии v0.6
Дата: 2026-11-01. Unreal Engine 5.8.4, Windows x64 Development.

## Исправления
1. В отдельной боевой арене декоративные колонны вынесены за линию обзора; длина плеча камеры уменьшена до 740 единиц Unreal.
2. Дальний круглый пол арены расширен по радиусу, чтобы убрать резкий обрыв поверхности.
3. Добавлен задний точечный заполняющий свет без теней для подсветки дальнего фона; основной и контровой свет сохранены.
4. У героя и врага на боевой арене остаются оригинальные скелетные меши и импортированные анимации, игровой интерфейс и механика таймингов не затронуты.
5. Проведена проверка подлинного кадра из Unreal Engine Editor и из самостоятельного Windows EXE; оба бойца видны одновременно, центральная преграда отсутствует.
6. Выпущена новая версия в отдельной папке Releases_v06, не перезаписывающая Releases_v05 или исходный Godot.

## Наблюдения и результаты
- Editor Win64 Development: PASS, E:\LastLightUnreal\UBT_EDITOR_V06_BACKDROP_BUILD.log
- Автотесты UE: 8 PASS / 0 FAIL, E:\LastLightUnreal\TestReportsV06\index.json
- Unreal AutomationTool BuildCookRun: BUILD SUCCESSFUL, E:\LastLightUnreal\UAT_V06_BUILDCOOKRUN.log
- Самостоятельная Windows-игра: Explore → Arena PASS, E:\LastLightUnreal\SMOKE_PACKAGED_ARENA_V06.log
- Визуальная проверка Unreal Frame: E:\LastLightUnreal\Releases_v06\Evidence\Arena_v06.png
- SHA-256 кадра: 4082861116617148ba1af769bc33b6d309ab99e0263c2fc06dec2b8c9e715a17
- Инвентаризация и контроль версий: 26/26 PASS, 50 файлов игровой папки, 950782921 байт
- Квитанция: E:\LastLightUnreal\Releases_v06\RELEASE_RECEIPT_UE_V06.json
- SHA-256 квитанции: 2de0f6b8d66148009e8927375d965cac8d3935e15a44ed2331dd86d5b1c143a1

## Ограничение проверки
Проверено: код компилируется, игровой EXE запускается, карты и персонажи действительно присутствуют, на изображении оба бойца видны, корпус боевой арены не перекрыт декорациями. Фактическое качество игры при WASD, ощущения таймингов, полный ручной путь до финального босса, Steam-release и стабильность FPS на разных GPU пока НЕ сертифицированы. Тёмный фон арены и низкая уникальность отдельных видов врагов остаются задачами визуальной полировки.

Источники UE API: https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/USpringArmComponent ; https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/FScreenshotRequest/RequestScreenshot
