#ifndef CPU_H
#define CPU_H

#include <cstdint>

extern uint16_t DIR;
extern uint8_t REJ_XY [2];
extern uint8_t REJ_ABC[4];
extern uint8_t REJ_IO;
extern uint16_t PIL[16];

uint8_t ALU(uint8_t OP,uint8_t A,uint8_t B,uint8_t des,bool no);
void REJ(uint8_t RSO,uint8_t RSI,uint8_t data,bool mode,uint8_t OP);
void DC(uint8_t J,uint8_t FLAG);
void PL(uint8_t FOP);
void PL_EXE(uint16_t PRG);

#endif