// secuencial.cpp
// Conteo de frecuencia de palabras - version secuencial
// Compilar: g++ -O2 -std=c++17 secuencial.cpp -o secuencial
// Ejecutar:  ./secuencial archivo.txt

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <chrono>

// Normaliza una palabra: minusculas y sin signos de puntuacion
std::string normalizar(const std::string& palabra) {
    std::string resultado;
    for (unsigned char c : palabra) {
        if (std::isalnum(c)) {
            resultado += std::tolower(c);
        }
    }
    return resultado;
}

// Lee el archivo completo y lo separa en una lista de palabras (paso "Split")
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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " archivo.txt\n";
        return 1;
    }

    // 1. Leer archivo + 2. Dividir en lista de palabras
    std::vector<std::string> palabras = leerYDividir(argv[1]);

    // Se mide solamente el conteo. La lectura, la normalizacion y la impresion
    // quedan fuera, igual que en la version paralela.
    auto inicio = std::chrono::steady_clock::now();

    // 3. Recorrer palabra por palabra actualizando el mapa de frecuencias
    std::unordered_map<std::string, int> frecuencias;
    for (const auto& palabra : palabras) {
        frecuencias[palabra]++;
    }

    auto fin = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(fin - inicio).count();

    // 4. Mostrar resultado final (ordenado alfabeticamente para poder comparar)
    std::vector<std::pair<std::string, int>> ordenado(frecuencias.begin(), frecuencias.end());
    std::sort(ordenado.begin(), ordenado.end());

    for (const auto& par : ordenado) {
        std::cout << par.first << ": " << par.second << "\n";
    }

    std::cerr << "\n[secuencial] palabras totales: " << palabras.size()
              << " | palabras distintas: " << frecuencias.size()
              << " | tiempo_conteo: " << ms << " ms\n"
              << "TIEMPO_MS=" << ms << "\n";

    return 0;
}
