# B06 - Pipeline GPU, Materiales/Shaders HLSL y Clínica de Optimización Empírica (Unreal Insights + RenderDoc)

Programación de pipeline GPU, shaders HLSL y optimización empírica de rendimiento: anatomía del pipeline de renderizado (GameThread -> RenderThread -> GPU, Deferred vs Forward, G-Buffer y coste real de Nanite/Lumen), Master Materials, nodos Custom HLSL y control desde C++ (UMaterialInstanceDynamic), seguido de 10 horas de clínica empírica con Unreal Insights y RenderDoc para diagnosticar y eliminar cuellos de botella de CPU y GPU con métricas reproducibles.

---

## Cápsulas de Sesión y Etiquetas de Paracaídas (`s2-b06-template`)

- **Proyecto / Toybox:** Proyecto 6 (Pieza 4): «Laboratorio HLSL + Clínica de Rescate GPU (22 FPS → 90 FPS)» (escena 3D sobrecargada con >35 ms/frame para perfilar con Unreal Insights y RenderDoc y bajar de 11 ms).
- **Capas 70 / 20 / 10:** • 70 % Base (en aula): Master Material con nodo Custom HLSL controlado desde C++ (UMaterialInstanceDynamic) + captura Antes/Después en Unreal Insights y RenderDoc bajando de >35 ms a <14 ms. · • 20 % Reto autónomo: Bajar el frame de la escena clínica a <11,1 ms (90 FPS sostenidos) combinando HISM, LODs y poda de Ticks/luces. · • 10 % Extra Portfolio: Informe de ingeniería estilo Tech Art / Rendering Engineer con capturas de pases de RenderDoc en inglés.
- **Plataforma docente (vídeo indexado y entregas):** [https://ikasyn.com/titulo-videojuegos](https://ikasyn.com/titulo-videojuegos)

| Sesión | Subbloque | Horas | Título | Tag Inicio (`sync_checkpoint`) | Tag Fin |
|:---:|:---:|:---:|---|---|---|
| `S2-20` | `B06.1` | 2 h | Pieza 4 (Shaders HLSL + Clínica GPU) · Anatomía del frame: GameThread → RenderThread → RHI/GPU | `scripts\sync_checkpoint.bat s2-19-start` | `s2-19-end` |
| `S2-21` | `B06.1` | 2 h | Pieza 4 · Coste real de Nanite, Lumen y Virtual Shadow Maps frente a presupuestos de 60/90 FPS | `scripts\sync_checkpoint.bat s2-20-start` | `s2-20-end` |
| `S2-22` | `B06.2` | 2 h | Pieza 4 · Arquitectura de Master Materials, instancias estáticas y coste de instrucciones de shader | `scripts\sync_checkpoint.bat s2-21-start` | `s2-21-end` |
| `S2-23` | `B06.2` | 2 h | Pieza 4 · Programación de Shaders en HLSL (Custom Node, SDFs, escáner 3D y World Position Offset) | `scripts\sync_checkpoint.bat s2-22-start` | `s2-22-end` |
| `S2-24` | `B06.2` | 2 h | Pieza 4 · Control de Shaders desde C++: UMaterialInstanceDynamic y Custom Primitive Data | `scripts\sync_checkpoint.bat s2-23-start` | `s2-23-end` |
| `S2-25` | `B06.3` | 2 h | Pieza 4 · Clínica de Perfilado I: captura y disección de CPU/hilos con Unreal Insights [Cierre materia nueva S2] | `scripts\sync_checkpoint.bat s2-24-start` | `s2-24-end` |
| `S2-26` | `B06.3` | 2 h | Pieza 4 · Clínica de Perfilado II: captura de frame e inspección de draw calls con RenderDoc (0 h casa) | `scripts\sync_checkpoint.bat s2-25-start` | `s2-25-end` |
| `S2-27` | `B06.3` | 2 h | Clínica en aula I (0 h casa): Instanciado en GPU (HISM/ISM), LODs y poda de Ticks en C++ | `scripts\sync_checkpoint.bat s2-26-start` | `s2-26-end` |
| `S2-28` | `B06.4` | 2 h | Clínica en aula II (0 h casa): Optimización de GPU (atenuación de luces, sombras y overdraw) | `scripts\sync_checkpoint.bat s2-27-start` | `s2-27-end` |
| `S2-29` | `B06.4` | 2 h | Clínica de cierre de S2 y defensa en puesto de la Pieza 4 (5 min, 0 h casa) antes de exámenes | `scripts\sync_checkpoint.bat s2-28-end` | `s2-28-end` |
