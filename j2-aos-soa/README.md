# J2 · AoS frente a SoA (S2, bloque 2.2)

`aos_soa.cpp` hace el mismo paso de simulación (posición, velocidad y rebote) sobre entidades de 64 bytes, de las que solo se usan 16. Se compara **Array of Structs** frente a **Struct of Arrays**, y se comprueba que los dos dan el mismo resultado bit a bit.

```text
g++ -std=c++20 -O2 aos_soa.cpp -o aos_soa && ./aos_soa 10000000
g++ -std=c++20 -O3 -march=native aos_soa.cpp -o aos_soa3 && ./aos_soa3 10000000
```

## Medido el 15-oct-2026 (revisado tras la auditoría D)

Equipo: AMD Ryzen 9 9950X (L1d 48 KiB, L2 1 MiB por núcleo, L3 32 MiB), WSL Ubuntu, g++ 13. Unidad: ns por entidad y paso. **5 repeticiones alternando el orden** (AoS primero en las impares, SoA primero en las pares); se da la **mediana** y el **rango** del cociente. Los cuatro campos se comparan bit a bit en todas las filas («iguales: sí»).

| Entidades | Memoria AoS | `-O2` AoS | `-O2` SoA | `-O2` AoS/SoA (rango) | `-O3 -march=native` AoS | SoA | AoS/SoA (rango) |
|---|---|---|---|---|---|---|---|
| 10.000 | 0,6 MB | 0,780 | 1,029 | **0,74** (0,72–0,94) | 0,612 | 0,121 | **5,07** (4,93–5,12) |
| 100.000 | 6,4 MB | 0,799 | 1,031 | **0,77** (0,76–0,81) | 0,682 | 0,141 | 4,85 (4,61–5,33) |
| 1.000.000 | 64 MB | 2,660 | 1,047 | **2,54** (2,39–2,69) | 3,150 | 0,224 | **14,1 (9,4–18,4)** |
| 10.000.000 | 640 MB | 3,402 | 1,190 | **2,79** (2,65–4,36) | 3,539 | 0,615 | 5,82 (5,32–5,92) |

**Lectura (el núcleo de la lección de 2.2):**
1. **Con `-O2` y datos que caben en caché (≤ 100k), SoA no gana; incluso pierde un poco.** Cuando los datos ya no caben (≥ 1M), la ventaja por ancho de banda de memoria es de unas 2,5–2,8×.
2. **Con `-O3 -march=native`, GCC vectoriza los dos bucles.** `-fopt-info-vec` informa de «loop vectorized using 64 byte vectors» tanto en `step_soa` como en `step_aos` (este último con accesos con salto). **La ventaja de SoA no viene de que un bucle tenga SIMD y el otro no**, sino de que SoA lee memoria **contigua** y mueve 16 bytes por entidad en lugar de 64: las cargas vectoriales son directas y no hay que reunir campos dispersos. Dentro de la caché la ventaja es de ≈ 5×.
3. **El pico de 1M (≈ 14×, de 9 a 18 según la ejecución) es propio de esta máquina:** es el tamaño en el que los 16 MB de SoA caben en los 32 MB de L3 y los 64 MB de AoS no. Con otra caché, el pico se desplaza o desaparece. **No se debe citar como cifra general**; lo general es «≈ 5× con SIMD dentro de la caché y 2–3× por ancho de banda fuera».
4. **«SoA siempre es mejor» es falso como regla.** Depende del tamaño frente a la caché, de qué campos se usan y del compilador. **Se mide antes de decidir, con repeticiones y rango.**

*La versión anterior de esta tabla (una sola ejecución, AoS siempre primero) daba 17,04× con 1M. La auditoría D reprodujo 13,4–17,5×. El método nuevo da un rango más amplio y una mediana menor.*

## Ajuste propuesto a la ficha de J2 en P3

La columna M de `P3_Bloques` dice «Enjambre de 10k a 100k entidades». En un equipo como este, **con ese rango y `-O2` no se ve ventaja de SoA.** Propuesta:
- J2 mide **de 10k a 10M entidades, con `-O2` y con `-O3 -march=native`**.
- El estudiante explica en el README dónde aparece la ventaja y por qué (caché y SIMD).

**Aviso para J1:** `-march=native` puede activar FMA y cambiar los bits de la simulación. En J1, la simulación determinista no usa `-march=native` ni `-ffast-math`.
