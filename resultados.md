# Resultados de rendimiento

Archivo utilizado: `grande.txt`. Cada configuracion se ejecuto 5 veces. Los tiempos corresponden solo al conteo (en paralelo incluyen fork, join y reduce).

## Tiempos

| Num. | Secuencial (ms) | Paralela 2 hilos (ms) | Paralela 4 hilos (ms) |
|---:|---:|---:|---:|
| 1 | 6.964940 | 5.737930 | 3.871920 |
| 2 | 9.021610 | 5.154870 | 3.476040 |
| 3 | 7.702570 | 5.556520 | 6.228910 |
| 4 | 6.930780 | 5.316180 | 3.540910 |
| 5 | 7.106290 | 5.192370 | 3.808280 |
| **Promedio** | **7.545238** | **5.391574** | **4.185212** |

## Speedup por ejecucion

Se usa `S(p) = T_secuencial / T_paralelo(p)`.

| Num. | Speedup 2 hilos | Speedup 4 hilos |
|---:|---:|---:|
| 1 | 1.2138 | 1.7988 |
| 2 | 1.7501 | 2.5954 |
| 3 | 1.3862 | 1.2366 |
| 4 | 1.3037 | 1.9573 |
| 5 | 1.3686 | 1.8660 |
| **Con promedios** | **1.3994** | **1.8028** |

## Efficiency por ejecucion

Se usa `E(p) = S(p) / p`; se muestra como porcentaje.

| Num. | Efficiency 2 hilos | Efficiency 4 hilos |
|---:|---:|---:|
| 1 | 60.69% | 44.97% |
| 2 | 87.51% | 64.88% |
| 3 | 69.31% | 30.91% |
| 4 | 65.19% | 48.93% |
| 5 | 68.43% | 46.65% |
| **Con promedios** | **69.97%** | **45.07%** |

## Datos para el analisis

- El mayor speedup promedio se obtuvo con 4 hilos.
- La mayor eficiencia promedio se obtuvo con 2 hilos.
- Un speedup menor que el numero de hilos indica que el escalamiento no fue proporcional.
- Posibles factores: planificacion del sistema operativo, carga del equipo, caches, asignaciones del mapa hash y variacion entre ejecuciones.
- El overhead paralelo incluye crear y unir hilos, mantener mapas locales y combinar sus resultados.
- Agregar hilos no garantiza mejorar: el trabajo adicional y la contencion por CPU o memoria pueden superar el ahorro del conteo concurrente.
