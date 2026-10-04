#include <cstdint>
#include <iostream>
#include <bitset>
#include <iomanip>
#include "libgame.h"
#include "memC.h"

bool FLAGS [4] = {0,0,0,0};
uint8_t REJ_ABC [4] = {0,0,0,0};
uint8_t REJ_XY [2] = {0,0};
uint8_t REJ_IO = 0;
uint16_t DIR = 0;
uint16_t PIL[16] = {0};

uint8_t YQS(uint8_t D,uint8_t DE,uint8_t REJ){
    if(D == 0 || DE ==  1){
        return (REJ << 1);
    }else if(D == 1 || DE == 2){
        return (REJ >> 1);
    }
    return REJ;
}


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

    
    OC = YQS(7,des,OC);

    if(no == 1) OC = ~OC;
    
    R = static_cast<uint8_t>(OC);
    
    REJ_ABC[DA]= R;
    return R;
} 

void REJ(uint8_t RSO,uint8_t RSI,uint8_t data,bool mode,uint8_t OP){
    if(OP == 1){ // SET

        REJ_ABC[RSI] = data;

    }else if(OP == 2){

        REJ_ABC[RSI] = REJ_ABC[RSO]; // MOVE

    }else if(OP == 3){

        REJ_XY[RSI] = data; //LOAD

    }else if(OP == 4){ //IOC

        if(mode == 0){

            REJ_ABC[RSO] = REJ_IO; //Rejistro comun es igual a Rejistro de interfas

        }else {

            REJ_IO = REJ_ABC[RSO]; //Rejistro de interfas es igal a un rejistro comun 

        }

    }
}



uint8_t SP = 0;
void DC(uint8_t J,uint8_t FLAG){
    if(J == 0){
        DIR++; //Funcion normal
    }else if(J == 1){
        DIR = (static_cast<uint16_t>(REJ_XY[1]) << 8) | REJ_XY[0]; //JMP
    }else if(J == 2){ //CJM (Condicional JMP)
        bool L = 0;
        switch(FLAG){
            case 0:
                if(FLAGS[0] == 1){
                    L = 1;
                }
                break;
            case 1:
                if(FLAGS[1] == 1){
                    L = 1;
                }
                break;
            case 2:
                if(FLAGS[2] == 1){
                    L = 1;
                }
                break;
            case 3:
                if(FLAGS[3] == 1){
                    L = 1;
                }
                break;
            default:
                break;
        }
        if(L == 1){
            DIR = (static_cast<uint16_t>(REJ_XY[1]) << 8) | REJ_XY[0]; //CJM
        }
    }else if (J == 3) { // CALL
    if (SP < 16) {
        DC(1,0);
        PIL[SP] = DIR;
        SP++;
    }

    } else if (J == 4) { // RETURN
        if (SP > 0) {
        SP--;
        DIR = PIL[SP];
        }
    }
    
}




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
        EscribirMemoria((static_cast<uint16_t>(REJ_XY[1]) <<8) | REJ_XY[0],REJ_IO); //WRM
        break;
    case 9:
        REJ_IO = LeerMemoria((static_cast<uint16_t>(REJ_XY[1]) <<8) | REJ_XY[0]); //RRM
        break;
    case 10:
        REJ((PRG >> 8) & 0b11,(PRG >> 6) & 0b11,0,0,2); // MOV
        break;
    case 11:
        REJ((PRG >> 8) & 0b11,0,0,(PRG >> 10) & 0b1,4); //IOC
        
        break;
    case 12:
        DC(1,0); //JMP
        break;
    case 13:
        DC(2,(PRG >>10) & 0b11); //CJP
        break;
    case 14:
        REJ(0,(PRG >> 8) & 0b11,PRG & 0xFF,0,1); // SET
        break;
    case 15:
        REJ(0,(PRG>> 10) & 0b1,PRG & 0xFF,0,3); // LOD
        
        break;
    case 16:
        REJ_ABC[(PRG >> 8) & 0b11] = YQS((PRG >> 10) & 0b1,0,REJ_ABC[(PRG >> 8) & 0b11]); // DES
        break;
    case 17:
        REJ_ABC[(PRG >> 8) & 0b11] = ~REJ_ABC[(PRG >> 8) & 0b11]; // NOP
        break;
    case 18:
        DC(3,0); //CALL
        break;
    case 19:
        DC(4,0); //RET
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