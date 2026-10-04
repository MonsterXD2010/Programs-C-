#include <fstream>
#include <string>
#include <cstdint>

uint8_t D_RAM[32768] = {0};
uint8_t O_RAM[32768] = {0};
uint8_t ROM[32768] = {0};

uint8_t PI;
uint8_t BancoRAM = 0;

uint8_t LeerMemoria(uint16_t DIR) {

    if (DIR < 0x8000) {
        // ROM
        PI = ROM[DIR];
    }
    else if (DIR >= 0x8000 && DIR <= 0xBFFF) {
        // RAM fija
        PI = D_RAM[DIR - 0x8000];
    }
    else if (DIR >= 0xC000 && DIR <= 0xCFFF) {
        // RAM bancaria

        uint16_t offset = DIR - 0xC000;

        if (BancoRAM < 8) {
            // Bancos 0-7 -> D_RAM
            PI = D_RAM[(BancoRAM * 4096) + offset];
        }
        else {
            // Bancos 8-15 -> O_RAM
            uint8_t banco = BancoRAM - 8;
            PI = O_RAM[(banco * 4096) + offset];
        }
    }

    return PI;
}

void EscribirMemoria(uint16_t DIR, uint8_t DATO) {

    if (DIR >= 0x8000 && DIR <= 0xBFFF) {
        D_RAM[DIR - 0x8000] = DATO;
    }
    else if (DIR >= 0xC000 && DIR <= 0xCFFF) {

        uint16_t offset = DIR - 0xC000;

        if (BancoRAM < 8) {
            D_RAM[(BancoRAM * 4096) + offset] = DATO;
        }
        else {
            uint8_t banco = BancoRAM - 8;
            O_RAM[(banco * 4096) + offset] = DATO;
        }
    }
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

