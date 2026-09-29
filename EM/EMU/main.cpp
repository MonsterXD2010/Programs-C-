#include <iostream>
#include <iomanip>
#include "CPU.h"
#include "libgame.h"
#include "memC.h"

int main()
{   
    gotoxy(0,0);
    OcultaCursor();
    Color(MORADO);
    printf("DIR:      REJ(A):    REJ(B):   REJ(C):        \nREJ(X):    REJ(Y):    DIR(XY):      ");
    CargarROM();
    while(1){
        
        gotoxy(4,0);
        Color(VERDE);
        std::cout << std::setfill('_') << std::setw(5) << DIR;
        gotoxy(17,0);
        Color(ROJO);
        std::cout <<std::setfill('0') << std::setw(3) << static_cast<int>(REJ(1,0,0,0,0,0,0,0));
        gotoxy(27,0);
        std::cout << std::setfill('0') << std::setw(3) << static_cast<int>(REJ(2,0,0,0,0,0,0,0));
        gotoxy(37,0);
        std::cout << std::setfill('0') << std::setw(3) << static_cast<int>(REJ(3,0,0,0,0,0,0,0));
        gotoxy(7,1);
        std::cout<< std::setfill('0') << std::setw(3) <<static_cast<int>(REJ_XY[0]);
        gotoxy(18,1);
        std::cout<< std::setfill('0') << std::setw(3) <<static_cast<int>(REJ_XY[1]);
        gotoxy(30,1);
        Color(VERDE);
        std::cout << ((static_cast<uint16_t>(REJ_XY[1]) << 8) | REJ_XY[0]);
        
        gotoxy(0,2);
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(LeerMemoria(DIR)) << std::dec;
        gotoxy(0,3);
        PL(LeerMemoria(DIR));
        pausa(0.00025);

        
    }
    
    return 0;
}