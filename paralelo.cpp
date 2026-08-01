// paralelo.cpp
// Conteo de frecuencia de palabras - version paralela (fork-join con hilos)
// Compilar: g++ -O2 -std=c++17 -pthread paralelo.cpp -o paralelo
// Ejecutar:  ./paralelo archivo.txt [num_hilos]

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <thread>
#include <chrono>

std::string normalizar(const std::string& palabra) {
    std::string resultado;
    for (unsigned char c : palabra) {
        if (std::isalnum(c)) {
            resultado += std::tolower(c);
        }
    }
    return resultado;
}

std::vector<std::string> leerYDividir(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo) {
        std::cerr << "No se pudo abrir el archivo: " << ruta << "\n";
        std::exit(1);
    }

    std::stringstream buffer;
    buffer << archivo.rdbuf();
    std::string texto = buffer.str();

    std::vector<std::string> palabras;
    std::istringstream stream(texto);
    std::string token;
    while (stream >> token) {
        std::string limpio = normalizar(token);
        if (!limpio.empty()) {
            palabras.push_back(limpio);
        }
    }
    return palabras;
}

// Tarea que ejecuta cada hilo: cuenta SOLO su propio rango [inicio, fin)
// y escribe en SU PROPIO mapa local -> no hay condicion de carrera,
// no se necesita mutex durante el conteo.
void contarBloque(const std::vector<std::string>& palabras,
                   size_t inicio, size_t fin,
                   std::unordered_map<std::string, int>& mapaLocal) {
    for (size_t i = inicio; i < fin; ++i) {
        mapaLocal[palabras[i]]++;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " archivo.txt [num_hilos]\n";
        return 1;
    }

    unsigned int numHilos = (argc >= 3)
        ? static_cast<unsigned int>(std::stoi(argv[2]))
        : std::max(2u, std::thread::hardware_concurrency());

    auto inicio = std::chrono::high_resolution_clock::now();

    // 1. Leer archivo + 2. Dividir en lista de palabras (igual que la version secuencial)
    std::vector<std::string> palabras = leerYDividir(argv[1]);

    if (palabras.empty()) {
        std::cerr << "El archivo no tiene palabras.\n";
        return 0;
    }
    if (numHilos > palabras.size()) numHilos = static_cast<unsigned int>(palabras.size());

    // 3. Dividir la lista en N sublistas (rangos), un mapa local por hilo
    std::vector<std::unordered_map<std::string, int>> mapasLocales(numHilos);
    std::vector<std::thread> hilos;

    size_t total = palabras.size();
    size_t tamBloque = total / numHilos;
    size_t sobrante = total % numHilos;

    size_t cursor = 0;
    for (unsigned int i = 0; i < numHilos; ++i) {
        size_t tam = tamBloque + (i < sobrante ? 1 : 0); // reparte el residuo
        size_t rangoInicio = cursor;
        size_t rangoFin = cursor + tam;
        cursor = rangoFin;

        // fork: cada hilo cuenta su bloque de forma independiente
        hilos.emplace_back(contarBloque, std::cref(palabras), rangoInicio, rangoFin,
                            std::ref(mapasLocales[i]));
    }

    // join: esperar a que todos los hilos terminen antes de combinar
    for (auto& h : hilos) {
        h.join();
    }

    // 4. Combinar (reduce): sumar todos los mapas locales en uno global
    std::unordered_map<std::string, int> frecuencias;
    for (auto& mapaLocal : mapasLocales) {
        for (auto& par : mapaLocal) {
            frecuencias[par.first] += par.second;
        }
    }

    auto fin = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(fin - inicio).count();

    // 5. Mostrar resultado final (mismo orden que la version secuencial, para comparar)
    std::vector<std::pair<std::string, int>> ordenado(frecuencias.begin(), frecuencias.end());
    std::sort(ordenado.begin(), ordenado.end());

    for (const auto& par : ordenado) {
        std::cout << par.first << ": " << par.second << "\n";
    }

    std::cerr << "\n[paralelo] hilos: " << numHilos
              << " | palabras totales: " << palabras.size()
              << " | palabras distintas: " << frecuencias.size()
              << " | tiempo: " << ms << " ms\n";

    return 0;
}
