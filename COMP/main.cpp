#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cctype>

uint32_t C_Count = 0;
uint32_t P_Count = 0;
std::string Code;
std::string Comp;
std::string P[5];
uint16_t Inst;
uint8_t F1;
uint8_t F2;
bool x = true;



struct Etiqueta
{
std::string Nombre;
uint32_t Direccion;
};

std::vector<Etiqueta> Etiquetas;

void Writer(std::string Archivo, uint32_t Direccion, uint8_t Dato)
{
// Crear/abrir el archivo en binario
std::fstream file(Archivo, std::ios::in | std::ios::out | std::ios::binary);

// Si no existe, crearlo y llenarlo con 32768 bytes
if (!file.is_open())
{
std::ofstream crear(Archivo, std::ios::binary);

if (!crear.is_open())
    return;

uint8_t cero = 0;

for (uint32_t i = 0; i < 32768; i++)
    crear.write(reinterpret_cast<char*>(&cero), 1);

crear.close();

// Volver a abrirlo para lectura/escritura
file.open(Archivo, std::ios::in | std::ios::out | std::ios::binary);

if (!file.is_open())
    return;

}

// Ir directamente a la dirección
file.seekp(Direccion);

// Escribir un byte
file.write(reinterpret_cast<char*>(&Dato), 1);

}

void Reader(std::string Archivo, int Lin)
{
    std::ifstream file(Archivo);

    x = true;

    if (!file.is_open())
    {
        x = false;
        return;
    }

    std::string linea;

    for (int i = 0; i <= Lin; i++)
    {
        if (!std::getline(file, linea))
        {
            x = false;
            return;
        }
    }

    // Quitar comentarios
    size_t comentario = linea.find(';');

    if (comentario != std::string::npos)
        linea.erase(comentario);

    // Quitar espacios del principio
    size_t inicio = linea.find_first_not_of(" \t");

    if (inicio == std::string::npos)
    {
        for (int i = 0; i < 5; i++)
            P[i] = "";

        return;
    }

    linea.erase(0, inicio);

    // Comprobar si hay una etiqueta
    size_t dosPuntos = linea.find(':');

    if (dosPuntos != std::string::npos)
    {
        linea.erase(0, dosPuntos + 1);

        // Quitar espacios después de la etiqueta
        inicio = linea.find_first_not_of(" \t");

        if (inicio == std::string::npos)
        {
            for (int i = 0; i < 5; i++)
                P[i] = "";

            return;
        }

        linea.erase(0, inicio);
    }

    // Limpiar palabras anteriores
    for (int i = 0; i < 5; i++)
        P[i] = "";

    // Separar la línea en palabras
    std::stringstream ss(linea);

    for (int i = 0; i < 5; i++)
    {
        if (!(ss >> P[i]))
            break;

        // Convertir a mayúsculas
        for (char &C : P[i])
            C = std::toupper(static_cast<unsigned char>(C));
    }
}

void LET(std::string A,uint8_t des){
    if(A == "A"){
        Inst |= 0b01 << des;
    }else if(A == "B"){
        Inst |= 0b10 << des;
    }else if(A == "C"){
        Inst |= 0b11 << des;
    }
}

void MOD(){

    if(P[3] == "L"){
        Inst |= 0b01 << 5;
    }else if(P[3] == "R"){
        Inst |= 0b10 << 5;
    }
    if(P[4] == "N" || P[3] == "N"){
        Inst |= 0b1 <<4;
    }

}


uint8_t NUM(std::string A,bool mode)
{
    uint16_t Valor;

    if (A[0] == '$')
        Valor = std::stoul(A.substr(1), nullptr, 16);
    else if (A[0] == '#')
        Valor = std::stoul(A.substr(1), nullptr, 10);
    else
        Valor = std::stoul(A, nullptr, 10);

    if(mode == 0){
        return Valor & 0xFF;
    }else if(mode == 1){
        return (Valor >> 8) & 0xFF;
    }
    return 0;
}

void CP(){
    F1 = Inst >> 8;
    F2 = Inst;

    Writer(Comp,C_Count,F1);
    C_Count++;

    Writer(Comp,C_Count,F2);
    C_Count++;
}
void LOD(uint8_t D,bool ñ){
    Inst |= 0b01111 << 11;

    if (ñ == 1)
        Inst |= 0b1 << 10;
    Inst |= D;
            
}

uint8_t BE(std::string N, bool X)
{
    for (const Etiqueta &E : Etiquetas)
    {
        if (E.Nombre == N)
        {
            if (X)
                return E.Direccion & 0xFF;
            else
                return E.Direccion >> 8;
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cout << "Uso: COMP <codigo> -o <resultado>\n";
        return 1;
    }

    Code = argv[1];

    if (std::string(argv[2]) != "-o")
    {
        std::cout << "Error: se esperaba -o\n";
        return 1;
    }

    Comp = argv[3];

    // =====================================================
    // PRIMERA PASADA
    // Buscar etiquetas y calcular sus direcciones
    // =====================================================

    Etiquetas.clear();

    std::ifstream archivo(Code);

    if (!archivo.is_open())
    {
        std::cout << "No se pudo abrir el archivo de codigo.\n";
        return 1;
    }

    std::string linea;
    uint32_t Direccion = 0;

    while (std::getline(archivo, linea))
    {
        // Quitar comentarios
        size_t comentario = linea.find(';');

        if (comentario != std::string::npos)
            linea.erase(comentario);

        // Quitar espacios del principio
        size_t inicio = linea.find_first_not_of(" \t");

        if (inicio == std::string::npos)
            continue;

        linea.erase(0, inicio);

        // Buscar etiqueta
        size_t dosPuntos = linea.find(':');

        if (dosPuntos != std::string::npos)
        {
            std::string Nombre = linea.substr(0, dosPuntos);

            // Quitar espacios al final del nombre
            size_t final = Nombre.find_last_not_of(" \t");

            if (final != std::string::npos)
                Nombre.erase(final + 1);

            Etiqueta Nueva;

            Nueva.Nombre = Nombre;
            Nueva.Direccion = Direccion;

            Etiquetas.push_back(Nueva);

            // Quitar la etiqueta para comprobar si queda una instrucción
            linea.erase(0, dosPuntos + 1);

            // Quitar espacios después de la etiqueta
            inicio = linea.find_first_not_of(" \t");

            if (inicio == std::string::npos)
                continue;

            linea.erase(0, inicio);
        }

        // Si después de quitar etiqueta/comentario no queda nada
        if (linea.empty())
            continue;

        // Hay una instrucción: cada instrucción ocupa 2 bytes
        Direccion += 2;
    }

    archivo.close();

    // =====================================================
    // SEGUNDA PASADA
    // Compilar
    // =====================================================

    C_Count = 0;
    P_Count = 0;
    x = true;

    while (x)
    {
        Reader(Code, P_Count);
        Inst = 0;

        if (!x)
            break;

        // Línea vacía, comentario o etiqueta sola
        // No genera bytes y no aumenta C_Count
        if (P[0] == "")
        {
            P_Count++;
            continue;
        }

        if (P[0] == "NOP" || P[0] == "")
        {
            Inst = 0;
        }
        else if (P[0] == "ADD")
        {
            Inst |= 0b00001 << 11;
            LET(P[1], 9);
            LET(P[2], 7);
            MOD();
        }
        else if (P[0] == "SUB")
        {
            Inst |= 0b00010 << 11;
            LET(P[1], 9);
            LET(P[2], 7);
            MOD();
        }
        else if (P[0] == "AND")
        {
            Inst |= 0b00011 << 11;
            LET(P[1], 9);
            LET(P[2], 7);
            MOD();
        }
        else if (P[0] == "OR")
        {
            Inst |= 0b00100 << 11;
            LET(P[1], 9);
            LET(P[2], 7);
            MOD();
        }
        else if (P[0] == "XOR")
        {
            Inst |= 0b00101 << 11;
            LET(P[1], 9);
            LET(P[2], 7);
            MOD();
        }
        else if (P[0] == "INC")
        {
            Inst |= 0b00110 << 11;
            LET(P[1], 9);
            LET(P[2], 7);
            MOD();
        }
        else if (P[0] == "DEC")
        {
            Inst |= 0b00111 << 11;
            LET(P[1], 9);
            LET(P[2], 7);
            MOD();
        }
        else if (P[0] == "WRM")
        {
            if(P[1] != ""){
                LOD(NUM(P[1],0),0);
                CP();
                Inst = 0;
                LOD(NUM(P[1],1),1);
                CP();
                Inst = 0;
            }
            Inst |= 0b01000 << 11;
        }
        else if (P[0] == "RRM")
        {   
            if(P[1] != ""){
                LOD(NUM(P[1],0),0);
                CP();
                Inst = 0;
                LOD(NUM(P[1],1),1);
                CP();
                Inst = 0;
            }
            Inst |= 0b01001 << 11;
        }
        else if (P[0] == "MOV")
        {
            Inst |= 0b01010 << 11;
            LET(P[1], 8);
            LET(P[2], 6);
        }
        else if (P[0] == "IOC")
        {
            Inst |= 0b01011 << 11;

            if (P[1] == "I")
                Inst |= 0b1 << 10;
            else if(P[1] == "O"){
                Inst |= 0b0 << 10;
            }
            LET(P[2], 8);
        }
        else if (P[0] == "JMP")
        {
            Inst |= 0b01100 << 11;
        }
        else if (P[0] == "CJP")
        {
            Inst |= 0b01101 << 11;

            // ZERO
            if (P[1] == "Z")
                Inst |= 0b00 << 10;

            // CARRY
            else if (P[1] == "C")
                Inst |= 0b01 << 10;

            // MENOR
            else if (P[1] == "ME")
                Inst |= 0b10 << 10;

            // MAYOR
            else if (P[1] == "MA")
            {    Inst |= 0b11 << 10;}
        }
        else if (P[0] == "SET")
        {
            Inst |= 0b01110 << 11;
            LET(P[1], 8);
            Inst |= NUM(P[2],0);
        }
        else if (P[0] == "LOD" || P[0] == "LOAD")
        {
            Inst |= 0b01111 << 11;

            if (P[1] == "Y")
                Inst |= 0b1 << 10;
            Inst |= NUM(P[2],0);
        }
        else if (P[0] == "DES")
        {
            Inst |= 0b10000 << 11;

            if (P[2] == "L")
                Inst |= 0b1 << 10;
            else if(P[2] == "R"){
                Inst |= 0b0 << 10;
            }
            LET(P[3], 8);
        }
        else if (P[0] == "NOT")
        {
            Inst |= 0b10001 << 11;
            LET(P[2], 8);
        }
        else if (P[0] == "CALL")
        {
            LOD(BE(P[1],1),0);
            CP();
            Inst = 0;
            LOD(BE(P[1],0),1);
            CP();
            Inst = 0;
            Inst |= 0b10010 << 11;
        }else if(P[0] == "RET"){
            Inst |= 0b10011 << 11;
        }

        CP();

        P_Count++;
    }
    std::cout<<"Numero de lineas: "<< P_Count << std::endl;
    std::cout<<"Bytes compilados: " << C_Count << std::endl;
    return 0;
}

