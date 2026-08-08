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

El valor `TIEMPO_MS` mide únicamente el conteo. La lectura, normalización y
presentación se excluyen en ambas versiones. En la versión paralela sí se
incluyen la creación y unión de hilos y la combinación de mapas, pues forman
parte del costo necesario para producir el resultado final.

## Experimento de rendimiento (5 ejecuciones)

```bash
chmod +x benchmark.sh
./benchmark.sh grande.txt
```

El script compila con optimización, realiza cinco pruebas secuenciales, cinco
con 2 hilos y cinco con 4 hilos usando el mismo archivo. Genera
`resultados.md` con los tiempos, promedios, speedup, efficiency y los elementos
para responder el análisis solicitado.

Las fórmulas utilizadas son:

- `Speedup(p) = tiempo secuencial / tiempo paralelo(p)`
- `Efficiency(p) = Speedup(p) / p`

## Resultados

Consulte [`resultados.md`](resultados.md) para ver la ejecución más reciente.
Los valores dependen del equipo y de la carga que tenga al hacer la prueba, por
lo que pueden regenerarse con el comando anterior.
