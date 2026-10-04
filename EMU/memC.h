#ifndef MEMC_H
#define MEMC_H

#include <cstdint>

extern uint8_t D_RAM[32768];
uint8_t LeerMemoria(uint16_t direccion);
void EscribirMemoria(uint16_t DIR, uint8_t DATO);
void CargarROM();

#endif