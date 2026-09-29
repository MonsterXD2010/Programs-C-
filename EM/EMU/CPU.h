#ifndef CPU_H
#define CPU_H

#include <cstdint>

extern uint16_t DIR;
extern uint8_t REJ_XY [2];
extern uint8_t REJ_D;

uint8_t ALU(uint8_t OP,uint8_t A,uint8_t B,uint8_t des,bool no);
uint8_t REJ(uint8_t SO,uint8_t IN,bool MOV,uint8_t M_SEL,uint8_t MOD,uint8_t DATA,uint8_t RAM,bool SET);
void DC(uint8_t J,uint8_t FLAG);
void PL(uint8_t FOP);
void PL_EXE(uint16_t PRG);

#endif