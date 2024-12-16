#include <iostream>
#include <ctime>

inline int elapsed(bool reset = false) {
    static clock_t start = clock();
    if (reset) {
        start = clock();
    }
    return static_cast<int>(1000.00 * double(clock() - start) / double(CLOCKS_PER_SEC));
}

int main() {
    std::cout << "Tiempo transcurrido: " << elapsed() << " ms" << std::endl;
    
    // Para reiniciar el temporizador
    std::cout << "Reiniciando el temporizador..." << std::endl;
        std::cout << "Reiniciando el temporizador..." << std::endl;

    std::cout << "Reiniciando el temporizador..." << std::endl;

    std::cout << "Reiniciando el temporizador..." << std::endl;


    std::cout << "Tiempo transcurrido después del reinicio: " << elapsed(true) << " ms" << std::endl;

    return 0;
}
