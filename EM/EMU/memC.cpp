#include <fstream>
#include <string>
#include <cstdint>


uint8_t RAM1[32768] = {0};
uint8_t RAM2[32768] = {0};
uint8_t ROM[32768] = {0};

uint8_t PI;

uint8_t LeerMemoria(uint16_t direccion) {

    if (direccion < 32768) {
        // ROM
        PI = ROM[direccion];
    }
    else {
        // RAM
        PI = RAM1[direccion - 32768];
    }

    return PI;
}

void CargarROM()
{
    std::ifstream archivo("rom.bin", std::ios::binary);

    if (!archivo.is_open())
        return;

    std::fill(std::begin(ROM), std::end(ROM), 0);

    archivo.read(
        reinterpret_cast<char*>(ROM),
        sizeof(ROM)
    );
}