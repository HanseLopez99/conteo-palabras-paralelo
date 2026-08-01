# Conteo de Frecuencia de Palabras — Secuencial vs Paralelo

Comparación entre una implementación secuencial y una paralela (fork-join con hilos) para el conteo de frecuencia de palabras en archivos de texto.

## Archivos

| Archivo | Descripción |
|---------|-------------|
| `secuencial.cpp` | Versión secuencial — recorre el archivo y cuenta palabras en un solo hilo |
| `paralelo.cpp` | Versión paralela — divide el texto en bloques, cada hilo cuenta su rango sin contención, luego combina los mapas locales |
| `prueba.txt` | Archivo pequeño de prueba (3 líneas) |
| `grande.txt` | Archivo grande de prueba (~300,000 palabras) |

## Compilación

```bash
g++ -O2 -std=c++17 secuencial.cpp -o secuencial
g++ -O2 -std=c++17 -pthread paralelo.cpp -o paralelo
```

## Ejecución

```bash
# Secuencial
./secuencial archivo.txt

# Paralelo (detecta automáticamente el número de hilos)
./paralelo archivo.txt

# Paralelo con número específico de hilos
./paralelo archivo.txt 4
```

## Resultados

Archivo de prueba: `grande.txt` (~300,000 palabras, 40 palabras distintas).

| Versión | Hilos | Tiempo |
|---------|-------|--------|
| Secuencial | 1 | ~30 ms |
| Paralelo | 1 | ~33 ms |
| Paralelo | 2 | ~34 ms |
| Paralelo | 4 | ~27 ms |
| Paralelo | 8 | ~27 ms |
| Paralelo | 16 | ~28 ms |

- Las salidas de ambas versiones son **idénticas**.
- Con **4+ hilos** la versión paralela supera a la secuencial (~10% más rápida).
- La mejora es moderada porque el archivo tiene solo 40 palabras distintas (la fase de merge es muy liviana).
