#include <cstdint>
#include <iostream>
#include <bitset>
#include <iomanip>
#include "libgame.h"

bool FLAGS [4] = {0,0,0,0};
uint8_t REJ_D = 0;
uint8_t REJ_ABC [4] = {0,0,0,0};
uint8_t REJ_XY [2] = {0,0};
uint8_t REJ_IO = 0;
uint16_t DIR = 0;

uint8_t ALU(uint8_t OP,uint8_t DA,uint8_t DB,uint8_t des,bool no){
    uint16_t OC;
    uint8_t R;

    uint8_t A = REJ_ABC[DA];
    uint8_t B = REJ_ABC[DB];

    switch (OP)
    {
    case 1:
        OC = A + B; //ADD
        break;
    case 2:
        OC = A - B; //SUB
        break;
    case 3:
        OC = A & B; //AND
        break;
    case 4:
        OC = A | B; //OR
        break;
    case 5:
        OC = A ^ B; //XOR
        break;
    case 6:
        OC = A + 1; //INC
        break;
    case 7:
        OC = A - 1; //DEC
        break;
    default:
        break;
    }

    if(OC > 255) FLAGS[1] = 1; //CARRY
    else FLAGS[1] = 0;
    if(OC == 0) FLAGS[0] = 1; //ZERO
    else FLAGS[0] = 0;
    if(FLAGS[0] == 1 && FLAGS[1] == 1) FLAGS[2] = 1; //MENOR
    else FLAGS[2] = 0;
    if(FLAGS[0] == 1 && FLAGS[1] == 0) FLAGS[3] = 1; //MAYOR
    else FLAGS[3] = 0;
    
    switch (des) //Desplasamientos
    {
    case 1:
        OC = OC << 1;
        break;
    case 2:
        OC = OC >> 1;
        break;
    default:
        break;
    }

    if(no == 1) OC = ~OC;
    
    R = static_cast<uint8_t>(OC);
    
    REJ_D= R;
    return R;
} 

uint8_t REJ(uint8_t SO,uint8_t IN,bool MOV,uint8_t M_SEL,uint8_t MOD,uint8_t DATA,uint8_t RAM,bool SET){
    
    if(MOD == 0){ // Copiar = CLN

        if(M_SEL == 1){  // REJ_ABC = D

            REJ_ABC[IN] = REJ_D;

        }else if(M_SEL == 2){ //Copiar valor de ram en registro IO.

            REJ_IO = RAM;

        }
    }else if(MOD == 1){

        if(M_SEL == 1){ //Copia balor de rej IO a un rejistro comun.

            REJ_ABC[IN] = REJ_IO;

        }else if(M_SEL == 2){ // Copia C en rej X.

            REJ_XY[0] = REJ_ABC[3];

        }else if(M_SEL == 3){ // Copia C en rej Y.

            REJ_XY[1] = REJ_ABC[3];

        }
    }
    if(MOV == 1) REJ_ABC[IN] = REJ_ABC[SO];REJ_ABC[0] = 0; //MOV
    if(SET == 1) REJ_ABC[IN] = DATA; REJ_ABC[0] = 0;
    switch (SO)
    {
    case 1:
        return REJ_ABC[1];
        break;
    case 2:
        return REJ_ABC[2];
        break;
    case 3:
        return REJ_ABC[3];
        break;
    default:
        return 0;
        break;
    }
    
}

void DC(uint8_t J,uint8_t FLAG){
    if(J == 0){
        DIR++; //Funcion normal
    }else if(J == 1){
        DIR = (static_cast<uint16_t>(REJ_XY[1]) << 8) | REJ_XY[0]; //JMP
    }else if(J == 2){ //CJM (Condicional JMP)
        uint8_t COM =   FLAGS[0] << 0 |
                        FLAGS[1] << 1 |
                        FLAGS[2] << 2 |
                        FLAGS[3] << 3;
        if(COM == FLAG){
            DIR = (static_cast<uint16_t>(REJ_XY[1]) << 8) | REJ_XY[0]; //CJM
        }
    }
    
}



//REJ(uint8_t SO,uint8_t IN,bool MOV,uint8_t M_SEL,uint8_t MOD,uint8_t DATA,uint8_t RAM)
void PL_EXE(uint16_t PRG){
    uint16_t OP = PRG >> 11;
    switch (OP)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        ALU(OP, ((PRG>>9) & 0b11),(PRG>>7) & 0b11,(PRG>>5)& 0b11,(PRG >> 4) & 0b1); // ALU
        break;
    case 8:
    case 9:
        break;
    case 10:
        REJ((PRG >> 8) & 0b11,(PRG>>6)& 0b11,1,0,0,0,0,0); // MOV
        break;
    case 11:
        REJ(0,(PRG >>6)& 0b11,0,(PRG >>8)& 0b11,(PRG >> 10)& 0b1,0,0,0); //CLN
        break;
    case 12:
        DC(1,0); //JMP
        break;
    case 13:
        DC(1,(PRG >>7) & 0b1111); //CJP
        break;
    case 14:
        REJ(0,(PRG >> 8)& 0b11,0,1,0,PRG & 0b11111111,0,1); // SET
        break;
    default:
        break;
    }
}

uint8_t F1;  // Fragmento 1
uint8_t F2;  // Fragmento 2
uint16_t TOP; //Total OP
uint8_t Count = 0;

void PL(uint8_t FOP){
    
     REJ_ABC[0] = 0;
    switch (Count)
    {
    case 0:
        F1 = FOP;
        DC(0,0);
        break;
    case 1:
        F2 = FOP;
        DC(0,0);
        break;
    case 2:
        TOP = (static_cast<uint16_t>(F1) << 8) | F2;
        break;
    case 3:
        std::cout << "\n" << std::bitset<16>(TOP);
        PL_EXE(TOP);
        Count = 255;

        break;
    default:
        break;
    }
    //std::cout << static_cast<int>(Count) << "\n\n";

    Count++;

}