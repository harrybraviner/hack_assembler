#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "replaceSymbols.h"

void populateSymbols(char** symbolList, uint16_t *symbolResolutions, int *numSymbols);

LineList replaceSymbols(LineList lineList) {
    LineList output = emptyLineList();

    char** symbolList = malloc(sizeof(char*)*(lineList.numberOfStoredLines+23));
    uint16_t* symbolResolution = malloc(sizeof(uint16_t)*(lineList.numberOfStoredLines+23));
    int numSymbols = 0;
    populateSymbols(symbolList, symbolResolution, &numSymbols);

    uint16_t instructionCount = 0;
    uint16_t nextMem = 16;

    for(int i=0; i < lineList.numberOfStoredLines; i++) {
        Line line = lineList.memory[i];
        if(line.lineType == LINETYPE_Label) {
            int symbolNumber = haveAlreadySeenSymbol(symbolList, numSymbols, line.label);
            if (symbolNumber > -1) {
                // This symbol needs to refer to an instruction - replace it's resolution with the relevant address in program memory
                symbolResolution[symbolNumber] = instructionCount;
            } else {
                symbolList[numSymbols] = malloc(sizeof(char)*50);
                strcpy(symbolList[numSymbols], line.label);
                symbolResolution[numSymbols] = instructionCount;
                numSymbols += 1;
            }
            printf("Replaced label %s with %d\n", line.label, instructionCount);
        } else {
            instructionCount++;
        }

        if(line.lineType == LINETYPE_A) {
            AInstruction aIns = line.aInstruction;
            if(!aIns.isLiteral) {
                int symbolNumber = haveAlreadySeenSymbol(symbolList, numSymbols, aIns.symbol);
                int thisResolution = -1;
                if(symbolNumber > -1) {
                    // We've already seen this symbol
                    thisResolution = symbolResolution[symbolNumber];
                } else {
                    // Need to add this symbol to the list
                    symbolList[numSymbols] = malloc(sizeof(char)*50);
                    strcpy(symbolList[numSymbols], aIns.symbol);
                    symbolResolution[numSymbols] = nextMem;
                    nextMem++;
                    thisResolution = nextMem;
                    numSymbols += 1;
                }
                printf("Replaced symbol %s in an A-instruction with %d\n", aIns.symbol, thisResolution);
                Line replacementLine = {lineType: LINETYPE_A,
                                        aInstruction: {isLiteral:1, literal: thisResolution}};
                addLine(&output, replacementLine);
            } else {
                addLine(&output, line);
            }
        }

        if (line.lineType == LINETYPE_C) {
            addLine(&output, line);
        }
    }

    return output;
}

int haveAlreadySeenSymbol(char** symbolList, int numSymbols, char* symbol) {
    for (int i=0; i<numSymbols; i++) {
        if(strcmp(symbol, symbolList[i])==0)
            return i;
    }
    return -1;
}

void populateSymbols(char** symbolList, uint16_t *symbolResolutions, int *numSymbols) {
    symbolList[0] = "SP";
    symbolResolutions[0] = 0;
    symbolList[1] = "LCL";
    symbolResolutions[1] = 1;
    symbolList[2] = "ARG";
    symbolResolutions[2] = 2;
    symbolList[3] = "THIS";
    symbolResolutions[3] = 3;
    symbolList[4] = "THAT";
    symbolResolutions[4] = 4;
    for(int i=0; i<16; i++) {
        symbolList[5+i] = malloc(sizeof(char)*50);
        sprintf(symbolList[5+i], "R%d", i);
        symbolResolutions[5+i] = i;
    }
    symbolList[21] = "SCREEN";
    symbolResolutions[21] = 16384;
    symbolList[22] = "KBD";
    symbolResolutions[22] = 24576;

    *numSymbols = 23;
}
