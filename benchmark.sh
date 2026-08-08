#!/usr/bin/env bash
set -euo pipefail

archivo="${1:-grande.txt}"
repeticiones=5

if [[ ! -f "$archivo" ]]; then
    echo "No existe el archivo de prueba: $archivo" >&2
    exit 1
fi

g++ -O2 -std=c++17 secuencial.cpp -o secuencial
g++ -O2 -std=c++17 -pthread paralelo.cpp -o paralelo

temporal="$(mktemp)"
trap 'rm -f "$temporal"' EXIT

obtener_tiempo() {
    local programa="$1"
    shift
    local salida
    salida=$("$programa" "$@" 2>&1 >/dev/null)
    awk -F= '/^TIEMPO_MS=/{print $2}' <<< "$salida"
}

echo "Ejecutando $repeticiones repeticiones con '$archivo'..." >&2
for ((i = 1; i <= repeticiones; i++)); do
    sec=$(obtener_tiempo ./secuencial "$archivo")
    par2=$(obtener_tiempo ./paralelo "$archivo" 2)
    par4=$(obtener_tiempo ./paralelo "$archivo" 4)
    printf '%d,%s,%s,%s\n' "$i" "$sec" "$par2" "$par4" >> "$temporal"
done

awk -F, -v archivo="$archivo" '
BEGIN {
    print "# Resultados de rendimiento"
    print ""
    print "Archivo utilizado: `" archivo "`. Cada configuracion se ejecuto 5 veces. Los tiempos corresponden solo al conteo (en paralelo incluyen fork, join y reduce)."
    print ""
    print "## Tiempos"
    print ""
    print "| Num. | Secuencial (ms) | Paralela 2 hilos (ms) | Paralela 4 hilos (ms) |"
    print "|---:|---:|---:|---:|"
}
{
    n[NR]=$1; s[NR]=$2; p2[NR]=$3; p4[NR]=$4
    ss+=$2; sp2+=$3; sp4+=$4
    printf "| %d | %.6f | %.6f | %.6f |\n", $1, $2, $3, $4
}
END {
    as=ss/NR; a2=sp2/NR; a4=sp4/NR
    printf "| **Promedio** | **%.6f** | **%.6f** | **%.6f** |\n", as, a2, a4
    print ""
    print "## Speedup por ejecucion"
    print ""
    print "Se usa `S(p) = T_secuencial / T_paralelo(p)`."
    print ""
    print "| Num. | Speedup 2 hilos | Speedup 4 hilos |"
    print "|---:|---:|---:|"
    for (i=1; i<=NR; i++) printf "| %d | %.4f | %.4f |\n", n[i], s[i]/p2[i], s[i]/p4[i]
    printf "| **Con promedios** | **%.4f** | **%.4f** |\n", as/a2, as/a4
    print ""
    print "## Efficiency por ejecucion"
    print ""
    print "Se usa `E(p) = S(p) / p`; se muestra como porcentaje."
    print ""
    print "| Num. | Efficiency 2 hilos | Efficiency 4 hilos |"
    print "|---:|---:|---:|"
    for (i=1; i<=NR; i++) printf "| %d | %.2f%% | %.2f%% |\n", n[i], 100*s[i]/p2[i]/2, 100*s[i]/p4[i]/4
    printf "| **Con promedios** | **%.2f%%** | **%.2f%%** |\n", 100*as/a2/2, 100*as/a4/4
    print ""
    print "## Datos para el analisis"
    print ""
    if (as/a2 > as/a4) print "- El mayor speedup promedio se obtuvo con 2 hilos."
    else print "- El mayor speedup promedio se obtuvo con 4 hilos."
    if (as/a2/2 > as/a4/4) print "- La mayor eficiencia promedio se obtuvo con 2 hilos."
    else print "- La mayor eficiencia promedio se obtuvo con 4 hilos."
    print "- Un speedup menor que el numero de hilos indica que el escalamiento no fue proporcional."
    print "- Posibles factores: planificacion del sistema operativo, carga del equipo, caches, asignaciones del mapa hash y variacion entre ejecuciones."
    print "- El overhead paralelo incluye crear y unir hilos, mantener mapas locales y combinar sus resultados."
    print "- Agregar hilos no garantiza mejorar: el trabajo adicional y la contencion por CPU o memoria pueden superar el ahorro del conteo concurrente."
}
' "$temporal" > resultados.md

echo "Resultados guardados en resultados.md" >&2
