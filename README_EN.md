CUM_8 Assembler

ALU OP
Format:
Operation REJ_A REJ_B Modifiers

Operations:
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

Modifiers:

    N = NOT
    L = Left shift
    R = Right shift

Communication with RAM and external devices

Format:
    Operation
    Operation Dir

Operations:
    WRM = Write the data from the IO register to the YX address
    RRM = Read the data from the YX address into the IO register

DIR Format:
    $0000   = Hexadecimal address
    #00000  = Decimal address
    00000   = Decimal address

Context:
    If you use WRM or RRM, it only takes the data from the YX address and points
    there, copying it to the IO register or from the IO register to the address.

    If you specify the address, it is almost the same, but the address is
    automatically set beforehand without having to do it manually.

Jumps
Format:
Operation
Operation dir/label

Operations:
    JMP
    CJP
    CALL
    RET

Context:
    JMP jumps to the specified address
    CJP jumps to a specified address if the flags match the specified ones
    CALL jumps to an address and saves its current position to return later
    RET, the opposite of CALL, takes the last saved position and jumps to it

CJP FLAGS:
    Z  = Zero
    C  = Carry
    Me = Less
    Ma = Greater

Place number

Format:
    Operation Reg num

Operations:
    SET
    LOD

REGS SET:
    A = 8b id:01
    B = 8b id:10
    C = 8b id:11

REGS LOD:

    X = 8b id 0
    Y = 8b id 1

Context:
    SET sets a value in a common 8-bit register A, B, or C
    LOD, also programmable as LOAD, is similar to SET, but handles
    the X and Y registers, which are the interface for controlling the memory address

Extra functions
Format:
Operation reg
Operation MODE REG

Operations:
    NOT
    DES

REGS:
    A = 8b id:01
    B = 8b id:10
    C = 8b id:11

Context:
    NOT inverts the bits. For example, 11110000 becomes 00001111 with this operation

    DES moves the bits 1 bit to the right or left. For example,
    a left shift would change 11110000 to 11100000,
    or a right shift would change 11110000 to 01111000