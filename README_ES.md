Ensamblador CUM_8

OP ALU
Formato:
Operación REJ_A REJ_B Modificadores

Operaciones:
    ADD = a + b
    SUB = a - b
    AND = a & b
    OR  = a | b
    XOR = a ^ b
    INC = a + 1
    DEC = a - 1

REJS:
    A = 8b id:01
    B = 8b id:10
    C = 8b id:11

Modificadores:

    N = NOT
    L = Desplazamiento a la izquierda
    R = Desplazamiento a la derecha

Comunicación con RAM y exterior

Formato:
    Operación
    Operación Dir

Operaciones:
    WRM = Escribir en la dirección YX el dato del registro IO
    RRM = Leer en la dirección YX el dato del registro IO

Formato DIR:
    $0000   = Dirección hexadecimal
    #00000  = Dirección decimal
    00000   = Dirección decimal

Contexto:
    Si pones WRM o RRM, solo toma el dato de la dirección YX y apunta allí,
    copiándolo al registro IO o del registro IO a la dirección.

    Si pones la dirección, es casi lo mismo, pero previamente se fija la
    dirección automáticamente sin hacerlo manualmente.

Saltos
Formato:
Operación
Operación dir/etiqueta

Operaciones:
    JMP
    CJP
    CALL
    RET

Contexto:
    JMP salta a la dirección especificada
    CJP salta a una dirección especificada si las banderas son iguales a las especificadas
    CALL salta a una dirección y guarda su posición actual para luego volver
    RET, lo contrario a CALL, toma la última posición guardada y salta hacia ella

BANDERAS CJP:
    Z  = Cero
    C  = Acarreo
    Me = Menor
    Ma = Mayor

Colocar número

Formato:
    Operación Rej num

Operaciones:
    SET
    LOD

REJS SET:
    A = 8b id:01
    B = 8b id:10
    C = 8b id:11

REJS LOD:

    X = 8b id 0
    Y = 8b id 1

Contexto:
    SET fija un valor en un registro común A, B o C de 8 bits
    LOD, o también programable como LOAD, es similar a SET, pero maneja
    los registros X e Y, que son la interfaz para controlar la dirección de memoria

Funciones extra
Formato:
Operación rej
Operación MODE REJ

Operaciones:
    NOT
    DES

REJS:
    A = 8b id:01
    B = 8b id:10
    C = 8b id:11

Contexto:
    NOT invierte los bits. Por ejemplo, 11110000 pasa a 00001111 con esta operación

    DES mueve los bits 1 bit a la derecha o a la izquierda. Por ejemplo,
    un desplazamiento a la izquierda sería de 11110000 a 11100000,
    o a la derecha de 11110000 a 01111000