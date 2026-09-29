#ifndef MEMC_H
#define MEMC_H

#include <cstdint>

extern uint8_t ROM[32768];
extern uint8_t RAM1[32768];
extern uint8_t RAM2[32768];

uint8_t LeerMemoria(uint16_t direccion);
void CargarROM();

#endif