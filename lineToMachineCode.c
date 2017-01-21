#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "lineToMachineCode.h"

uint16_t compCodeToInt(Comp comp);

uint16_t lineToMachineCode(Line line) {
    if (line.lineType == LINETYPE_Label) {
        fprintf(stderr, "Error! lineToMachineCode does not expect to see any labels!\n");
        exit(-1);
    }

    if (line.lineType == LINETYPE_A) {
        if (line.aInstruction.isLiteral == 0) {
            fprintf(stderr, "Error! lineToMachineCode does not expect to see symbolic (i.e. non-literal) A-instructions!\n");
            exit(-1);
        }
        return line.aInstruction.literal;
    }

    if (line.lineType == LINETYPE_C) {
        uint16_t comp = compCodeToInt(line.cInstruction.comp);
        uint16_t jump = line.cInstruction.jump.j3 + 2*line.cInstruction.jump.j2 + 4*line.cInstruction.jump.j1;
        uint16_t dest = line.cInstruction.dest.d3 + 2*line.cInstruction.dest.d2 + 4*line.cInstruction.dest.d1;
        return ((0b111 << 13) + (comp << 6) + (dest << 3) + jump);
    }

    fprintf(stderr, "Error: unknown linetype.\n");
    exit(-1);
}

uint16_t compCodeToInt(Comp comp) {
    switch(comp) {
        case COMP_zero    : return 0b0101010; break;
        case COMP_one     : return 0b0111111; break;
        case COMP_minusOne: return 0b0111010; break;
        case COMP_D       : return 0b0001100; break;
        case COMP_A       : return 0b0110000; break;
        case COMP_notD    : return 0b0001101; break;
        case COMP_notA    : return 0b0110001; break;
        case COMP_negD    : return 0b0001111; break;
        case COMP_negA    : return 0b0110011; break;
        case COMP_incD    : return 0b0011111; break;
        case COMP_incA    : return 0b0110111; break;
        case COMP_decD    : return 0b0001110; break;
        case COMP_decA    : return 0b0110010; break;
        case COMP_DplusA  : return 0b0000010; break;
        case COMP_DminusA : return 0b0010011; break;
        case COMP_AminusD : return 0b0000111; break;
        case COMP_DandA   : return 0b0000000; break;
        case COMP_DorA    : return 0b0010101; break;

        case COMP_M       : return 0b1110000; break;
        case COMP_notM    : return 0b1110001; break;
        case COMP_negM    : return 0b1110011; break;
        case COMP_incM    : return 0b1110111; break;
        case COMP_decM    : return 0b1110010; break;
        case COMP_DplusM  : return 0b1000010; break;
        case COMP_DminusM : return 0b1010011; break;
        case COMP_MminusD : return 0b1000111; break;
        case COMP_DandM   : return 0b1000000; break;
        case COMP_DorM    : return 0b1010101; break;
    }

    fprintf(stderr, "Error: Unrecognised Comp enum value.\n");
    exit(-1);
}
